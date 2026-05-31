#pragma once
#ifndef __FLZ_AGENT_LLM_DEEPSEEK_CLIENT_H__
#define __FLZ_AGENT_LLM_DEEPSEEK_CLIENT_H__

/**
 * @file    deepseek_client.h
 * @brief   DeepSeek LLM 客户端（当前最小可用实现）
 * @date    2026-05
 * @note    RAII: 连接池交由 flz::HttpConnectionPool 管理
 */

#include "agent/llm/i_llm_client.h"

#include <string>

namespace agent {

class DeepseekClient : public ILlmClient {
public:
    explicit DeepseekClient(const std::string& provider_name);
    int streamChat(const std::vector<LlmMessage>& messages,
                   const DeltaCallback& on_delta,
                   Json::Value* out_result = nullptr) override;

private:
    std::string m_providerName;
};

} // namespace agent

#endif // __FLZ_AGENT_LLM_DEEPSEEK_CLIENT_H__
