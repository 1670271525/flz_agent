#include "agent/session/session_context.h"

namespace agent {

void SessionContext::reset(const std::vector<ChatMessagePo>& history) {
    m_messages = history;
}

void SessionContext::append(const ChatMessagePo& msg) {
    m_messages.push_back(msg);
}

void SessionContext::trimByRounds(int max_rounds) {
    if (max_rounds <= 0) {
        m_messages.clear();
        return;
    }
    const size_t keep = static_cast<size_t>(max_rounds * 2);
    if (m_messages.size() <= keep) {
        return;
    }
    m_messages.erase(m_messages.begin(), m_messages.end() - keep);
}

const std::vector<ChatMessagePo>& SessionContext::messages() const {
    return m_messages;
}

} // namespace agent
