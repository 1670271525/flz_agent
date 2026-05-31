#pragma once
#ifndef __FLZ_AGENT_LLM_I_LLM_CLIENT_H__
#define __FLZ_AGENT_LLM_I_LLM_CLIENT_H__

/**
 * @file    i_llm_client.h
 * @brief   LLM 客户端接口
 * @date    2026-05
 * @note    RAII: 接口对象由 shared_ptr 管理生命周期
 */

#include "agent/llm/llm_message.h"

#include <functional>
#include <json/json.h>
#include <memory>
#include <vector>

namespace agent {

class ILlmClient {
public:
    typedef std::shared_ptr<ILlmClient> ptr;
    typedef std::function<bool(const std::string& delta)> DeltaCallback;
    virtual ~ILlmClient() {}
    virtual int streamChat(const std::vector<LlmMessage>& messages,
                           const DeltaCallback& on_delta,
                           Json::Value* out_result = nullptr) = 0;
};

} // namespace agent

#endif // __FLZ_AGENT_LLM_I_LLM_CLIENT_H__
