#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct CliConfig {
    std::string url = "http://127.0.0.1:8090/v1/chat/sse";
    std::string sessionId = "chat_cli";
    std::string agentType = "chat";
    std::string token;
};

struct ParsedUrl {
    std::string host;
    int port = 80;
    std::string path = "/";
};

struct SseStats {
    std::map<std::string, int> eventCounts;
    bool hasError = false;
};

static void printUsage(const char* program) {
    std::cout
        << "用法: " << program << " [--url URL] [--session SESSION_ID] [--agent AGENT_TYPE] [--token JWT]\n"
        << "\n示例:\n"
        << "  " << program << " --url http://127.0.0.1:8090/v1/chat/sse --session chat_test\n"
        << "\n运行后支持命令:\n"
        << "  /help                 查看命令\n"
        << "  /session <id>         切换会话ID\n"
        << "  /agent <type>         切换 agent_type\n"
        << "  /token <jwt>          设置 Bearer Token\n"
        << "  /show                 查看当前配置\n"
        << "  /quit                 退出\n"
        << std::endl;
}

static bool startsWith(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

static std::string trim(const std::string& s) {
    size_t begin = 0;
    while (begin < s.size() && (s[begin] == ' ' || s[begin] == '\t' || s[begin] == '\r' || s[begin] == '\n')) {
        ++begin;
    }
    size_t end = s.size();
    while (end > begin && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n')) {
        --end;
    }
    return s.substr(begin, end - begin);
}

static std::string jsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 16);
    for (size_t i = 0; i < s.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        switch (c) {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                if (c < 0x20) {
                    char buf[8];
                    std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out += buf;
                } else {
                    out += static_cast<char>(c);
                }
                break;
        }
    }
    return out;
}

static bool parseUrl(const std::string& url, ParsedUrl& out) {
    if (!startsWith(url, "http://")) {
        std::cerr << "[错误] 当前仅支持 http:// URL: " << url << std::endl;
        return false;
    }
    std::string rest = url.substr(7);
    size_t slash = rest.find('/');
    std::string host_port = (slash == std::string::npos) ? rest : rest.substr(0, slash);
    out.path = (slash == std::string::npos) ? "/" : rest.substr(slash);
    if (out.path.empty()) {
        out.path = "/";
    }

    size_t colon = host_port.rfind(':');
    if (colon == std::string::npos) {
        out.host = host_port;
        out.port = 80;
        return !out.host.empty();
    }
    out.host = host_port.substr(0, colon);
    std::string p = host_port.substr(colon + 1);
    if (out.host.empty() || p.empty()) {
        return false;
    }
    out.port = std::atoi(p.c_str());
    return out.port > 0 && out.port <= 65535;
}

static int connectTo(const ParsedUrl& u) {
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* result = nullptr;
    std::stringstream port_ss;
    port_ss << u.port;
    int rc = getaddrinfo(u.host.c_str(), port_ss.str().c_str(), &hints, &result);
    if (rc != 0) {
        std::cerr << "[错误] getaddrinfo 失败: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int fd = -1;
    for (struct addrinfo* rp = result; rp != nullptr; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0) {
            continue;
        }
        if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        close(fd);
        fd = -1;
    }
    freeaddrinfo(result);
    return fd;
}

static bool sendAll(int fd, const std::string& data) {
    size_t sent = 0;
    while (sent < data.size()) {
        ssize_t n = send(fd, data.data() + sent, data.size() - sent, 0);
        if (n <= 0) {
            return false;
        }
        sent += static_cast<size_t>(n);
    }
    return true;
}

static void printConfig(const CliConfig& cfg) {
    std::cout << "[配置] url=" << cfg.url
              << " session=" << cfg.sessionId
              << " agent=" << cfg.agentType
              << " token=" << (cfg.token.empty() ? "<空>" : "<已设置>")
              << std::endl;
}

static void dispatchSseEvent(const std::string& event, const std::string& data, SseStats& stats) {
    const std::string evt = event.empty() ? "message" : event;
    stats.eventCounts[evt]++;
    if (evt == "error") {
        stats.hasError = true;
    }

    std::time_t now = std::time(nullptr);
    char ts[32];
    std::strftime(ts, sizeof(ts), "%H:%M:%S", std::localtime(&now));
    std::cout << "[" << ts << "] " << evt << " => " << data << std::endl;
}

static bool parseHttpStatus(const std::string& status_line, int& status_code) {
    std::stringstream ss(status_line);
    std::string http_ver;
    ss >> http_ver;
    ss >> status_code;
    return !http_ver.empty() && status_code > 0;
}

static bool readSseResponse(int fd, SseStats& stats) {
    std::string recv_buf;
    recv_buf.reserve(8192);
    char tmp[2048];
    bool header_done = false;
    std::string status_line;

    std::string current_event;
    std::vector<std::string> data_lines;

    auto flush_event = [&]() {
        if (current_event.empty() && data_lines.empty()) {
            return;
        }
        std::stringstream ss;
        for (size_t i = 0; i < data_lines.size(); ++i) {
            if (i) {
                ss << "\n";
            }
            ss << data_lines[i];
        }
        dispatchSseEvent(current_event, ss.str(), stats);
        current_event.clear();
        data_lines.clear();
    };

    while (true) {
        ssize_t n = recv(fd, tmp, sizeof(tmp), 0);
        if (n == 0) {
            flush_event();
            return true;
        }
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            std::cerr << "[错误] recv 失败: " << std::strerror(errno) << std::endl;
            return false;
        }
        recv_buf.append(tmp, static_cast<size_t>(n));

        if (!header_done) {
            size_t p = recv_buf.find("\r\n\r\n");
            if (p == std::string::npos) {
                continue;
            }
            std::string headers = recv_buf.substr(0, p);
            recv_buf.erase(0, p + 4);

            size_t line_end = headers.find("\r\n");
            status_line = (line_end == std::string::npos) ? headers : headers.substr(0, line_end);
            int status_code = 0;
            if (!parseHttpStatus(status_line, status_code)) {
                std::cerr << "[错误] 非法状态行: " << status_line << std::endl;
                return false;
            }
            std::cout << "[HTTP] " << status_line << std::endl;
            if (status_code != 200) {
                std::cout << "[HTTP] 响应体: " << recv_buf << std::endl;
                return false;
            }
            header_done = true;
        }

        size_t line_pos = std::string::npos;
        while ((line_pos = recv_buf.find('\n')) != std::string::npos) {
            std::string line = recv_buf.substr(0, line_pos);
            recv_buf.erase(0, line_pos + 1);
            if (!line.empty() && line[line.size() - 1] == '\r') {
                line.erase(line.size() - 1);
            }

            if (line.empty()) {
                flush_event();
                continue;
            }
            if (startsWith(line, "event:")) {
                current_event = trim(line.substr(6));
            } else if (startsWith(line, "data:")) {
                data_lines.push_back(trim(line.substr(5)));
            } else if (startsWith(line, ":")) {
                // 心跳注释，忽略
            }
        }
    }
}

static bool requestOnce(const CliConfig& cfg, const std::string& msg, SseStats& stats) {
    ParsedUrl parsed;
    if (!parseUrl(cfg.url, parsed)) {
        return false;
    }
    int fd = connectTo(parsed);
    if (fd < 0) {
        std::cerr << "[错误] 连接失败: " << parsed.host << ":" << parsed.port << std::endl;
        return false;
    }

    const std::string body =
        std::string("{\"session_id\":\"") + jsonEscape(cfg.sessionId)
        + "\",\"msg\":\"" + jsonEscape(msg)
        + "\",\"agent_type\":\"" + jsonEscape(cfg.agentType) + "\"}";

    std::stringstream req;
    req << "POST " << parsed.path << " HTTP/1.1\r\n";
    req << "Host: " << parsed.host << ":" << parsed.port << "\r\n";
    req << "Accept: text/event-stream\r\n";
    req << "Content-Type: application/json\r\n";
    req << "Connection: close\r\n";
    if (!cfg.token.empty()) {
        req << "Authorization: Bearer " << cfg.token << "\r\n";
    }
    req << "Content-Length: " << body.size() << "\r\n\r\n";
    req << body;

    const std::string req_data = req.str();
    if (!sendAll(fd, req_data)) {
        std::cerr << "[错误] 发送请求失败" << std::endl;
        close(fd);
        return false;
    }

    bool ok = readSseResponse(fd, stats);
    close(fd);
    return ok;
}

static bool parseArgs(int argc, char** argv, CliConfig& cfg) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            std::exit(0);
        }
        if (i + 1 >= argc) {
            std::cerr << "[错误] 缺少参数值: " << arg << std::endl;
            return false;
        }
        const std::string val = argv[++i];
        if (arg == "--url") {
            cfg.url = val;
        } else if (arg == "--session") {
            cfg.sessionId = val;
        } else if (arg == "--agent") {
            cfg.agentType = val;
        } else if (arg == "--token") {
            cfg.token = val;
        } else {
            std::cerr << "[错误] 未知参数: " << arg << std::endl;
            return false;
        }
    }
    return true;
}

} // namespace

int main(int argc, char** argv) {
    CliConfig cfg;
    if (!parseArgs(argc, argv, cfg)) {
        return 1;
    }

    std::cout << "=== flz_agent 终端链路测试工具 ===" << std::endl;
    printConfig(cfg);
    std::cout << "输入消息后回车即可发起 SSE 请求，输入 /help 查看命令。" << std::endl;

    std::string line;
    while (true) {
        std::cout << "\n> ";
        if (!std::getline(std::cin, line)) {
            std::cout << "\n输入结束，退出。" << std::endl;
            break;
        }
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        if (line == "/quit" || line == "/exit") {
            break;
        }
        if (line == "/help") {
            printUsage(argv[0]);
            continue;
        }
        if (line == "/show") {
            printConfig(cfg);
            continue;
        }
        if (startsWith(line, "/session ")) {
            cfg.sessionId = trim(line.substr(9));
            std::cout << "[OK] session=" << cfg.sessionId << std::endl;
            continue;
        }
        if (startsWith(line, "/agent ")) {
            cfg.agentType = trim(line.substr(7));
            std::cout << "[OK] agent=" << cfg.agentType << std::endl;
            continue;
        }
        if (startsWith(line, "/token ")) {
            cfg.token = trim(line.substr(7));
            std::cout << "[OK] token 已更新" << std::endl;
            continue;
        }

        std::cout << "[请求] session=" << cfg.sessionId << " agent=" << cfg.agentType << std::endl;
        SseStats stats;
        const bool ok = requestOnce(cfg, line, stats);
        if (!ok) {
            std::cout << "[结果] 请求失败" << std::endl;
            continue;
        }
        std::cout << "[结果] 事件统计:";
        for (std::map<std::string, int>::const_iterator it = stats.eventCounts.begin();
             it != stats.eventCounts.end(); ++it) {
            std::cout << " " << it->first << "=" << it->second;
        }
        if (stats.hasError) {
            std::cout << " (含 error 事件)";
        }
        std::cout << std::endl;
    }

    return 0;
}
