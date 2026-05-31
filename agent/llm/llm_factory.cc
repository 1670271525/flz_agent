#include "agent/llm/llm_factory.h"

#include "agent/config/llm_config.h"
#include "agent/llm/deepseek_client.h"

namespace agent {

ILlmClient::ptr LlmFactory::create(const std::string& provider_name) {
    std::string selected = provider_name;
    if (selected.empty()) {
        selected = LlmConfig::get().defaultProvider;
    }
    return ILlmClient::ptr(new DeepseekClient(selected));
}

} // namespace agent
