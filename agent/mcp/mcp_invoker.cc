#include "agent/mcp/mcp_invoker.h"

#include "agent/mcp/mcp_registry.h"
#include "agent/util/json_helper.h"
#include "http/http_connection.h"
#include "include/log.h"

#include <map>
#include <string>

namespace agent {

static flz::Logger::ptr g_logger = FLZ_LOG_ROOT();

bool McpInvoker::invoke(const std::string& tool_name, const Json::Value& args, Json::Value& result) {
    McpToolDescriptor desc;
    if (!McpRegistry::GetInstance()->findTool(tool_name, desc)) {
        result["name"] = tool_name;
        result["ok"] = false;
        result["error"] = "tool not found";
        return false;
    }
    if (desc.transport != "http") {
        result["name"] = tool_name;
        result["ok"] = false;
        result["error"] = "unsupported transport";
        result["transport"] = desc.transport;
        return false;
    }

    Json::Value req;
    req["tool"] = tool_name;
    req["name"] = tool_name;
    req["arguments"] = args;

    std::map<std::string, std::string> headers;
    headers["Content-Type"] = "application/json";

    flz::http::HttpResult::ptr http_result =
        flz::http::HttpConnection::DoPost(desc.endpoint, static_cast<uint64_t>(desc.timeoutMs), headers,
                                          JsonHelper::toString(req));

    result["name"] = tool_name;
    result["service"] = desc.serviceName;
    result["endpoint"] = desc.endpoint;
    if (!http_result || !http_result->response) {
        result["ok"] = false;
        result["error"] = http_result ? http_result->error : "null response";
        FLZ_LOG_ERROR(g_logger) << "invoke tool fail, name=" << tool_name
                                << " endpoint=" << desc.endpoint
                                << " error=" << result["error"].asString();
        return false;
    }

    const int status = static_cast<int>(http_result->response->getStatus());
    Json::Value body_json;
    const std::string body = http_result->response->getBody();
    const bool body_is_json = JsonHelper::parse(body, body_json);

    result["status"] = status;
    if (body_is_json) {
        result["data"] = body_json;
    } else {
        result["data"] = body;
    }

    if (status >= 200 && status < 300) {
        result["ok"] = true;
        return true;
    }

    result["ok"] = false;
    if (!body.empty()) {
        result["error"] = body;
    } else {
        result["error"] = "http status not success";
    }
    return false;
}

} // namespace agent
