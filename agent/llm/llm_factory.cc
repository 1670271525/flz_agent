#include "agent/llm/llm_factory.h"

#include "agent/llm/deepseek_client.h"

namespace agent {

ILlmClient::ptr LlmFactory::create(const std::string& provider_name) {
    (void)provider_name;
    return ILlmClient::ptr(new DeepseekClient());
}

} // namespace agent
