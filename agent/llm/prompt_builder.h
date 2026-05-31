#pragma once
#ifndef __FLZ_AGENT_LLM_PROMPT_BUILDER_H__
#define __FLZ_AGENT_LLM_PROMPT_BUILDER_H__

/**
 * @file    prompt_builder.h
 * @brief   Prompt 组装器
 * @date    2026-05
 * @note    RAII: 构造结果为值对象，不持有外部句柄
 */

#include "agent/dao/chat_message_po.h"
#include "agent/llm/llm_message.h"

#include <string>
#include <vector>

namespace agent {

class PromptBuilder {
public:
    static std::vector<LlmMessage> build(const std::vector<ChatMessagePo>& messages,
                                         const std::string& agent_type,
                                         const std::string& tool_schema_json);
};

} // namespace agent

#endif // __FLZ_AGENT_LLM_PROMPT_BUILDER_H__
