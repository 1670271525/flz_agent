#include "agent/mcp/tool_call_parser.h"

#include "agent/util/json_helper.h"

namespace agent {

namespace {

static bool parseArgs(const Json::Value& src, Json::Value& out) {
    if (src.isObject() || src.isArray()) {
        out = src;
        return true;
    }
    if (src.isString()) {
        Json::Value parsed;
        if (JsonHelper::parse(src.asString(), parsed)) {
            out = parsed;
        } else {
            out = Json::Value(Json::objectValue);
            out["raw"] = src.asString();
        }
        return true;
    }
    out = Json::Value(Json::objectValue);
    return true;
}

static const Json::Value* resolveToolCallArray(const Json::Value& llm_json) {
    if (llm_json.isMember("tool_calls") && llm_json["tool_calls"].isArray()) {
        return &llm_json["tool_calls"];
    }
    if (llm_json.isMember("message") && llm_json["message"].isObject()
        && llm_json["message"].isMember("tool_calls")
        && llm_json["message"]["tool_calls"].isArray()) {
        return &llm_json["message"]["tool_calls"];
    }
    if (llm_json.isMember("choices") && llm_json["choices"].isArray()
        && !llm_json["choices"].empty()
        && llm_json["choices"][0].isMember("message")
        && llm_json["choices"][0]["message"].isObject()
        && llm_json["choices"][0]["message"].isMember("tool_calls")
        && llm_json["choices"][0]["message"]["tool_calls"].isArray()) {
        return &llm_json["choices"][0]["message"]["tool_calls"];
    }
    return nullptr;
}

} // namespace

bool ToolCallParser::parse(const Json::Value& llm_json, std::vector<ToolCall>& out_calls) {
    const Json::Value* arr = resolveToolCallArray(llm_json);
    if (!arr) {
        return true;
    }

    for (Json::ArrayIndex i = 0; i < arr->size(); ++i) {
        const Json::Value& item = (*arr)[i];
        ToolCall call;
        if (item.isMember("function") && item["function"].isObject()) {
            call.name = item["function"].get("name", "").asString();
            parseArgs(item["function"]["arguments"], call.arguments);
        } else {
            call.name = item.get("name", "").asString();
            parseArgs(item["arguments"], call.arguments);
        }
        if (!call.name.empty()) {
            out_calls.push_back(call);
        }
    }
    return true;
}

} // namespace agent
