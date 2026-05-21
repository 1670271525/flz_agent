#include "agent/mcp/tool_call_parser.h"

namespace agent {

bool ToolCallParser::parse(const Json::Value& llm_json, std::vector<ToolCall>& out_calls) {
    if (!llm_json.isMember("tool_calls") || !llm_json["tool_calls"].isArray()) {
        return true;
    }
    const Json::Value& arr = llm_json["tool_calls"];
    for (Json::ArrayIndex i = 0; i < arr.size(); ++i) {
        ToolCall call;
        call.name = arr[i].get("name", "").asString();
        call.arguments = arr[i]["arguments"];
        if (!call.name.empty()) {
            out_calls.push_back(call);
        }
    }
    return true;
}

} // namespace agent
