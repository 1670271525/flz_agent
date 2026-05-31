#include "agent/llm/deepseek_client.h"

#include "agent/config/llm_config.h"
#include "agent/llm/sse_event_parser.h"
#include "agent/util/json_helper.h"
#include "http/http_connection.h"
#include "http/http_parser.h"
#include "include/log.h"
#include "include/socket.h"
#include "include/uri.h"

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace agent {

static flz::Logger::ptr g_logger = FLZ_LOG_ROOT();

namespace {

struct ToolCallState {
    std::string name;
    std::string arguments;
};

struct StreamAggregate {
    std::vector<ToolCallState> tool_states;
    std::string all_text;
    std::string finish_reason = "stop";
    Json::Value usage;
};

static std::string trim(const std::string& s) {
    size_t begin = 0;
    while (begin < s.size() && (s[begin] == ' ' || s[begin] == '\t' || s[begin] == '\r' || s[begin] == '\n')) {
        ++begin;
    }
    size_t end = s.size();
    while (end > begin
           && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n')) {
        --end;
    }
    return s.substr(begin, end - begin);
}

static std::string extractDataPayload(const std::string& event_text) {
    std::stringstream ss(event_text);
    std::string line;
    std::string data;
    while (std::getline(ss, line)) {
        std::string t = trim(line);
        if (t.size() < 5 || t.substr(0, 5) != "data:") {
            continue;
        }
        std::string payload = trim(t.substr(5));
        if (!data.empty()) {
            data.append("\n");
        }
        data.append(payload);
    }
    return data;
}

static Json::Value tryParseJsonOrString(const std::string& raw) {
    Json::Value out;
    if (JsonHelper::parse(raw, out)) {
        return out;
    }
    Json::Value text;
    text = raw;
    return text;
}

static int processSseEvents(const std::vector<std::string>& events,
                            const ILlmClient::DeltaCallback& on_delta,
                            StreamAggregate& agg) {
    for (size_t i = 0; i < events.size(); ++i) {
        const std::string payload = extractDataPayload(events[i]);
        if (payload.empty()) {
            continue;
        }
        if (payload == "[DONE]") {
            break;
        }

        Json::Value evt;
        if (!JsonHelper::parse(payload, evt)) {
            continue;
        }
        if (evt.isMember("usage") && evt["usage"].isObject()) {
            agg.usage = evt["usage"];
        }
        if (!evt.isMember("choices") || !evt["choices"].isArray() || evt["choices"].empty()) {
            continue;
        }

        const Json::Value& choice0 = evt["choices"][0];
        if (choice0.isMember("finish_reason") && !choice0["finish_reason"].isNull()) {
            agg.finish_reason = choice0["finish_reason"].asString();
        }
        if (!choice0.isMember("delta") || !choice0["delta"].isObject()) {
            continue;
        }
        const Json::Value& delta = choice0["delta"];
        const std::string delta_text = delta.get("content", "").asString();
        if (!delta_text.empty()) {
            agg.all_text.append(delta_text);
            if (!on_delta(delta_text)) {
                return -6;
            }
        }
        if (!delta.isMember("tool_calls") || !delta["tool_calls"].isArray()) {
            continue;
        }
        const Json::Value& tc_arr = delta["tool_calls"];
        for (Json::ArrayIndex j = 0; j < tc_arr.size(); ++j) {
            const Json::Value& tc = tc_arr[j];
            int idx = tc.get("index", static_cast<int>(j)).asInt();
            if (idx < 0) {
                continue;
            }
            if (agg.tool_states.size() <= static_cast<size_t>(idx)) {
                agg.tool_states.resize(static_cast<size_t>(idx + 1));
            }
            const Json::Value& fn = tc["function"];
            if (fn.isObject()) {
                const std::string name_part = fn.get("name", "").asString();
                if (!name_part.empty()) {
                    agg.tool_states[static_cast<size_t>(idx)].name = name_part;
                }
                const std::string args_part = fn.get("arguments", "").asString();
                if (!args_part.empty()) {
                    agg.tool_states[static_cast<size_t>(idx)].arguments.append(args_part);
                }
            }
        }
    }
    return 0;
}

static void fillOutResult(const StreamAggregate& agg, Json::Value* out_result) {
    if (!out_result) {
        return;
    }
    Json::Value message;
    message["role"] = "assistant";
    message["content"] = agg.all_text;
    Json::Value tool_calls(Json::arrayValue);
    for (size_t i = 0; i < agg.tool_states.size(); ++i) {
        if (agg.tool_states[i].name.empty()) {
            continue;
        }
        Json::Value tc;
        tc["name"] = agg.tool_states[i].name;
        tc["arguments"] = tryParseJsonOrString(agg.tool_states[i].arguments);
        tool_calls.append(tc);
    }
    if (!tool_calls.empty()) {
        message["tool_calls"] = tool_calls;
    }
    (*out_result)["message"] = message;
    (*out_result)["finish_reason"] = agg.finish_reason;
    if (agg.usage.isObject() && !agg.usage.empty()) {
        (*out_result)["usage"] = agg.usage;
    } else {
        Json::Value usage;
        usage["prompt_tokens"] = 0;
        usage["completion_tokens"] = 0;
        usage["total_tokens"] = 0;
        (*out_result)["usage"] = usage;
    }
}

/**
 * 增量读取 HTTP 响应体；每收到一段解码后的正文即回调 on_body_chunk。
 * 逻辑与 flz::http::HttpConnection::recvResponse 一致，但不缓冲完整 body。
 */
static int recvResponseBodyStream(flz::http::HttpConnection::ptr conn,
                                  const std::function<int(const std::string&)>& on_body_chunk,
                                  int* out_http_status) {
    flz::http::HttpResponseParser::ptr parser(new flz::http::HttpResponseParser);
    const uint64_t buff_size = flz::http::HttpResponseParser::GetHttpResponseBufferSize();
    std::shared_ptr<char> buffer(new char[buff_size + 1], [](char* ptr) { delete[] ptr; });
    char* data = buffer.get();
    int offset = 0;

    do {
        int len = conn->read(data + offset, buff_size - offset);
        if (len <= 0) {
            conn->close();
            return -1;
        }
        len += offset;
        data[len] = '\0';
        const size_t nparse = parser->execute(data, len, false);
        if (parser->hasError()) {
            conn->close();
            return -2;
        }
        offset = len - static_cast<int>(nparse);
        if (offset == static_cast<int>(buff_size)) {
            conn->close();
            return -3;
        }
        if (parser->isFinished()) {
            break;
        }
    } while (true);

    if (out_http_status && parser->getData()) {
        *out_http_status = static_cast<int>(parser->getData()->getStatus());
    }

    const httpclient_parser& client_parser = parser->getParser();
    if (client_parser.chunked) {
        int len = offset;
        do {
            bool begin = true;
            do {
                if (!begin || len == 0) {
                    const int rt = conn->read(data + len, buff_size - len);
                    if (rt <= 0) {
                        conn->close();
                        return -4;
                    }
                    len += rt;
                }
                data[len] = '\0';
                const size_t nparse = parser->execute(data, len, true);
                if (parser->hasError()) {
                    conn->close();
                    return -5;
                }
                len -= static_cast<int>(nparse);
                if (len == static_cast<int>(buff_size)) {
                    conn->close();
                    return -6;
                }
                begin = false;
            } while (!parser->isFinished());

            if (static_cast<int>(client_parser.content_len) + 2 <= len) {
                const std::string chunk(data, client_parser.content_len);
                if (!chunk.empty() && on_body_chunk(chunk) != 0) {
                    conn->close();
                    return -7;
                }
                memmove(data, data + client_parser.content_len + 2,
                        len - client_parser.content_len - 2);
                len -= static_cast<int>(client_parser.content_len + 2);
            } else {
                std::string chunk(data, static_cast<size_t>(len));
                int left = static_cast<int>(client_parser.content_len) - len + 2;
                while (left > 0) {
                    const int rt = conn->read(data, left > static_cast<int>(buff_size)
                                                     ? static_cast<int>(buff_size)
                                                     : left);
                    if (rt <= 0) {
                        conn->close();
                        return -8;
                    }
                    chunk.append(data, static_cast<size_t>(rt));
                    left -= rt;
                }
                if (chunk.size() >= 2) {
                    chunk.resize(chunk.size() - 2);
                }
                if (!chunk.empty() && on_body_chunk(chunk) != 0) {
                    conn->close();
                    return -9;
                }
                len = 0;
            }
        } while (!client_parser.chunks_done);
    } else {
        int64_t length = static_cast<int64_t>(parser->getContentLength());
        if (length > 0) {
            std::string body;
            body.reserve(static_cast<size_t>(length));
            if (offset > 0) {
                const size_t copy_len = static_cast<size_t>(length) < static_cast<size_t>(offset)
                                            ? static_cast<size_t>(length)
                                            : static_cast<size_t>(offset);
                body.append(data, copy_len);
                length -= static_cast<int64_t>(copy_len);
            }
            while (length > 0) {
                const size_t want = static_cast<size_t>(length) > buff_size ? buff_size
                                                                            : static_cast<size_t>(length);
                const int rt = conn->read(data, want);
                if (rt <= 0) {
                    conn->close();
                    return -10;
                }
                body.append(data, static_cast<size_t>(rt));
                length -= rt;
            }
            if (!body.empty() && on_body_chunk(body) != 0) {
                conn->close();
                return -11;
            }
        } else if (offset > 0) {
            if (on_body_chunk(std::string(data, static_cast<size_t>(offset))) != 0) {
                conn->close();
                return -12;
            }
        }
    }
    return 0;
}

static flz::http::HttpConnection::ptr openPostConnection(const std::string& url,
                                                         uint64_t timeout_ms,
                                                         const std::map<std::string, std::string>& headers,
                                                         const std::string& body,
                                                         std::string* err_msg) {
    flz::Uri::ptr uri = flz::Uri::Create(url);
    if (!uri) {
        if (err_msg) {
            *err_msg = "invalid url: " + url;
        }
        return nullptr;
    }
    const bool is_ssl = uri->getScheme() == "https";
    flz::Address::ptr addr = uri->createAddress();
    if (!addr) {
        if (err_msg) {
            *err_msg = "invalid host: " + uri->getHost();
        }
        return nullptr;
    }
    flz::Socket::ptr sock =
        is_ssl ? flz::SSLSocket::CreateTCP(addr) : flz::Socket::CreateTCP(addr);
    if (!sock) {
        if (err_msg) {
            *err_msg = "create socket fail";
        }
        return nullptr;
    }
    if (!sock->connect(addr)) {
        if (err_msg) {
            *err_msg = "connect fail: " + addr->toString();
        }
        return nullptr;
    }
    sock->setRecvTimeout(timeout_ms);

    flz::http::HttpRequest::ptr req = std::make_shared<flz::http::HttpRequest>();
    req->setPath(uri->getPath());
    req->setQuery(uri->getQuery());
    req->setFragment(uri->getFragment());
    req->setMethod(flz::http::HttpMethod::POST);
    bool has_host = false;
    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end();
         ++it) {
        if (strcasecmp(it->first.c_str(), "connection") == 0) {
            if (strcasecmp(it->second.c_str(), "keep-alive") == 0) {
                req->setClose(false);
            }
            continue;
        }
        if (!has_host && strcasecmp(it->first.c_str(), "host") == 0) {
            has_host = !it->second.empty();
        }
        req->setHeader(it->first, it->second);
    }
    if (!has_host) {
        req->setHeader("Host", uri->getHost());
    }
    req->setBody(body);

    flz::http::HttpConnection::ptr conn = std::make_shared<flz::http::HttpConnection>(sock);
    const int rt = conn->sendRequest(req);
    if (rt == 0) {
        if (err_msg) {
            *err_msg = "send request closed by peer";
        }
        return nullptr;
    }
    if (rt < 0) {
        if (err_msg) {
            *err_msg = "send request socket error";
        }
        return nullptr;
    }
    return conn;
}

} // namespace

DeepseekClient::DeepseekClient(const std::string& provider_name)
    : m_providerName(provider_name) {
}

int DeepseekClient::streamChat(const std::vector<LlmMessage>& messages,
                               const DeltaCallback& on_delta,
                               Json::Value* out_result) {
    if (!on_delta || messages.empty()) {
        return -1;
    }

    LlmProviderConfig cfg = LlmConfig::getProvider(m_providerName);
    if (cfg.baseUrl.empty() || cfg.ssePath.empty() || cfg.modelId.empty()) {
        FLZ_LOG_ERROR(g_logger) << "deepseek config invalid, provider=" << m_providerName;
        return -2;
    }

    std::string api_key = cfg.apiKey;
    if (api_key.empty()) {
        FLZ_LOG_ERROR(g_logger) << "deepseek api_key empty, provider=" << m_providerName
                                << "; if llm.yml uses ${DEEPSEEK_API_KEY}, run: export DEEPSEEK_API_KEY=sk-xxx"
                                << " (jwt.yml secret is NOT the DeepSeek API key)";
        return -3;
    }

    Json::Value req_json;
    req_json["model"] = cfg.modelId;
    req_json["stream"] = true;
    req_json["stream_options"]["include_usage"] = true;
    req_json["temperature"] = 0.2;
    req_json["messages"] = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < messages.size(); ++i) {
        Json::Value msg;
        msg["role"] = messages[i].role;
        msg["content"] = messages[i].content;
        if (!messages[i].toolCallJson.empty()) {
            Json::Value tool_calls;
            if (JsonHelper::parse(messages[i].toolCallJson, tool_calls) && tool_calls.isArray()) {
                msg["tool_calls"] = tool_calls;
            }
        }
        req_json["messages"].append(msg);
    }

    const std::string url = cfg.baseUrl + cfg.ssePath;
    std::map<std::string, std::string> headers;
    headers["Content-Type"] = "application/json";
    headers["Accept"] = "text/event-stream";
    headers["Authorization"] = "Bearer " + api_key;

    std::string conn_err;
    flz::http::HttpConnection::ptr conn =
        openPostConnection(url, cfg.timeoutMs, headers, JsonHelper::toString(req_json), &conn_err);
    if (!conn) {
        FLZ_LOG_ERROR(g_logger) << "deepseek connect failed, url=" << url << " err=" << conn_err;
        return -4;
    }

    SseEventParser parser;
    StreamAggregate agg;
    agg.usage["prompt_tokens"] = 0;
    agg.usage["completion_tokens"] = 0;
    agg.usage["total_tokens"] = 0;

    int http_status = 0;
    const int body_rt = recvResponseBodyStream(
        conn,
        [&](const std::string& chunk) -> int {
            const std::vector<std::string> events = parser.append(chunk);
            return processSseEvents(events, on_delta, agg);
        },
        &http_status);

    conn->close();

    if (body_rt < 0) {
        FLZ_LOG_ERROR(g_logger) << "deepseek stream read failed, url=" << url
                                << " body_rt=" << body_rt;
        return -5;
    }
    if (http_status >= 400) {
        FLZ_LOG_ERROR(g_logger) << "deepseek http status=" << http_status;
        return -6;
    }

    fillOutResult(agg, out_result);
    return 0;
}

} // namespace agent
