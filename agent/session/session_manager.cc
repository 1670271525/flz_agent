#include "agent/session/session_manager.h"

namespace agent {

std::shared_ptr<flz::Mutex> SessionManager::getSessionLock(const std::string& session_id) {
    RWMutexType::WriteLock lock(m_mutex);
    std::map<std::string, std::weak_ptr<flz::Mutex> >::iterator it = m_locks.find(session_id);
    if (it != m_locks.end()) {
        std::shared_ptr<flz::Mutex> sp = it->second.lock();
        if (sp) {
            return sp;
        }
    }
    std::shared_ptr<flz::Mutex> created(new flz::Mutex());
    m_locks[session_id] = created;
    return created;
}

} // namespace agent
