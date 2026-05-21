#include "agent/llm/prompt_builder.h"

namespace agent {

std::vector<LlmMessage> PromptBuilder::build(const std::vector<ChatMessagePo>& history,
                                             const std::string& user_msg,
                                             const std::string& tool_schema_json) {
    std::vector<LlmMessage> out;
    LlmMessage sys;
    sys.role = "system";
    sys.content = "你是 flz_agent。可用工具定义: " + tool_schema_json;
    out.push_back(sys);

    for (size_t i = 0; i < history.size(); ++i) {
        LlmMessage m;
        m.role = history[i].role;
        m.content = history[i].content;
        out.push_back(m);
    }

    LlmMessage user;
    user.role = "user";
    user.content = user_msg;
    out.push_back(user);
    return out;
}

} // namespace agent
