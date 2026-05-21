#pragma once
#ifndef __FLZ_AGENT_SESSION_SESSION_CONTEXT_H__
#define __FLZ_AGENT_SESSION_SESSION_CONTEXT_H__

/**
 * @file    session_context.h
 * @brief   会话上下文窗口管理
 * @date    2026-05
 * @note    RAII: 持有消息值对象，不管理外部资源
 */

#include "agent/dao/chat_message_po.h"

#include <vector>

namespace agent {

class SessionContext {
public:
    void reset(const std::vector<ChatMessagePo>& history);
    void append(const ChatMessagePo& msg);
    void trimByRounds(int max_rounds);
    const std::vector<ChatMessagePo>& messages() const;

private:
    std::vector<ChatMessagePo> m_messages;
};

} // namespace agent

#endif // __FLZ_AGENT_SESSION_SESSION_CONTEXT_H__
