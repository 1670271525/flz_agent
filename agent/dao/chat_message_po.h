#pragma once
#ifndef __FLZ_AGENT_DAO_CHAT_MESSAGE_PO_H__
#define __FLZ_AGENT_DAO_CHAT_MESSAGE_PO_H__

/**
 * @file    chat_message_po.h
 * @brief   聊天消息持久化对象定义
 * @date    2026-05
 * @note    RAII: 值对象，无外部资源持有
 */

#include <stdint.h>
#include <string>

namespace agent {

struct ChatMessagePo {
    uint64_t id = 0;
    std::string agentId;
    std::string sessionId;
    uint64_t userId = 0;
    std::string agentName = "flz_agent";
    std::string agentType = "chat";
    std::string agentVersion = "1.0";
    int agentStatus = 1;
    std::string role;
    std::string content;
    std::string toolName;
    std::string toolArgsJson;
    std::string toolResultJson;
    int tokenPrompt = 0;
    int tokenCompletion = 0;
    std::string extJson;
};

} // namespace agent

#endif // __FLZ_AGENT_DAO_CHAT_MESSAGE_PO_H__
