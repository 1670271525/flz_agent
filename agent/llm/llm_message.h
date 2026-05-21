#pragma once
#ifndef __FLZ_AGENT_LLM_LLM_MESSAGE_H__
#define __FLZ_AGENT_LLM_LLM_MESSAGE_H__

/**
 * @file    llm_message.h
 * @brief   LLM 消息 DTO
 * @date    2026-05
 * @note    RAII: 值对象，不持有外部资源
 */

#include <string>

namespace agent {

struct LlmMessage {
    std::string role;
    std::string content;
    std::string toolCallJson;
};

} // namespace agent

#endif // __FLZ_AGENT_LLM_LLM_MESSAGE_H__
