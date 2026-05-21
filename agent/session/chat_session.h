#pragma once
#ifndef __FLZ_AGENT_SESSION_CHAT_SESSION_H__
#define __FLZ_AGENT_SESSION_CHAT_SESSION_H__

/**
 * @file    chat_session.h
 * @brief   单次请求会话 RAII，负责 session 级串行锁生命周期
 * @date    2026-05
 * @note    RAII: 构造加锁，析构自动解锁
 */

#include "include/mutex.h"

#include <memory>
#include <string>

namespace agent {

class ChatSession {
public:
    typedef std::shared_ptr<ChatSession> ptr;

    ChatSession(const std::string& session_id, uint64_t user_id);
    ~ChatSession();

    ChatSession(const ChatSession&) = delete;
    ChatSession& operator=(const ChatSession&) = delete;

    const std::string& sessionId() const;
    uint64_t userId() const;

private:
    std::string m_sessionId;
    uint64_t m_userId = 0;
    std::shared_ptr<flz::Mutex> m_sessionLock;
    std::unique_ptr<flz::Mutex::Lock> m_guard;
};

} // namespace agent

#endif // __FLZ_AGENT_SESSION_CHAT_SESSION_H__
