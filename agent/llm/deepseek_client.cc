#include "agent/llm/deepseek_client.h"

namespace agent {

int DeepseekClient::streamChat(const std::vector<LlmMessage>& messages, const DeltaCallback& on_delta) {
    if (!on_delta) {
        return -1;
    }
    if (!messages.empty()) {
        on_delta("暂未接入真实 DeepSeek，上游链路已预留。");
    }
    return 0;
}

} // namespace agent
