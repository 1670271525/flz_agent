#pragma once
#ifndef __FLZ_AGENT_LLM_LLM_FACTORY_H__
#define __FLZ_AGENT_LLM_LLM_FACTORY_H__

/**
 * @file    llm_factory.h
 * @brief   LLM 工厂，根据 model/provider 返回客户端实例
 * @date    2026-05
 * @note    RAII: 工厂仅创建 shared_ptr，不持有连接资源
 */

#include "agent/llm/i_llm_client.h"

#include <string>

namespace agent {

class LlmFactory {
public:
    static ILlmClient::ptr create(const std::string& provider_name);
};

} // namespace agent

#endif // __FLZ_AGENT_LLM_LLM_FACTORY_H__
