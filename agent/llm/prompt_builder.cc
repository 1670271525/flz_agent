#include "agent/llm/prompt_builder.h"

namespace agent {

namespace {

static std::string buildSystemPrompt(const std::string& agent_type,
                                     const std::string& tool_schema_json) {
    const bool has_tools = !tool_schema_json.empty() && tool_schema_json != "[]";
    if (agent_type == "mcp" && has_tools) {
        return "你是 flz_agent，可根据用户需求调用以下工具完成任务。可用工具: " + tool_schema_json;
    }
    return "你是一个友善、专业的 AI 助手。请用简洁清晰的中文与用户自然对话，"
           "回答问题、提供建议或闲聊。不要主动声称自己能控制智能家居、家电或任何外部设备；"
           "若用户未明确要求使用工具，请以普通对话方式回应。";
}

} // namespace

std::vector<LlmMessage> PromptBuilder::build(const std::vector<ChatMessagePo>& messages,
                                             const std::string& agent_type,
                                             const std::string& tool_schema_json) {
    std::vector<LlmMessage> out;
    LlmMessage sys;
    sys.role = "system";
    sys.content = buildSystemPrompt(agent_type, tool_schema_json);
    out.push_back(sys);

    for (size_t i = 0; i < messages.size(); ++i) {
        LlmMessage m;
        m.role = messages[i].role;
        m.content = messages[i].content;
        m.toolCallJson = messages[i].toolArgsJson;
        out.push_back(m);
    }
    return out;
}

} // namespace agent
