#include "agent/session/chat_session.h"

#include "agent/session/session_manager.h"
#include "include/log.h"

namespace agent {

static flz::Logger::ptr g_logger = FLZ_LOG_ROOT();

ChatSession::ChatSession(const std::string& session_id, uint64_t user_id)
    : m_sessionId(session_id)
    , m_userId(user_id)
    , m_sessionLock(SessionManagerSingleton::GetInstance()->getSessionLock(session_id))
    , m_guard(new flz::Mutex::Lock(*m_sessionLock)) {
    FLZ_LOG_INFO(g_logger) << "ChatSession acquire lock, session_id=" << session_id << " user_id=" << user_id;
}

ChatSession::~ChatSession() {
    m_guard.reset();
    FLZ_LOG_INFO(g_logger) << "ChatSession release lock, session_id=" << m_sessionId;
}

const std::string& ChatSession::sessionId() const {
    return m_sessionId;
}

uint64_t ChatSession::userId() const {
    return m_userId;
}

} // namespace agent
