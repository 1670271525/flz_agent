#include "agent/mcp/mcp_invoker.h"

namespace agent {

bool McpInvoker::invoke(const std::string& tool_name, const Json::Value& args, Json::Value& result) {
    result["name"] = tool_name;
    result["ok"] = true;
    result["echo_args"] = args;
    return true;
}

} // namespace agent
