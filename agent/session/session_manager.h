#pragma once
#ifndef __FLZ_AGENT_SESSION_SESSION_MANAGER_H__
#define __FLZ_AGENT_SESSION_SESSION_MANAGER_H__

/**
 * @file    session_manager.h
 * @brief   会话管理单例，提供 session 级串行锁
 * @date    2026-05
 * @note    RAII: 锁对象由 shared_ptr 托管，引用计数归零自动释放
 */

#include "include/mutex.h"
#include "include/singleton.h"

#include <map>
#include <memory>
#include <string>

namespace agent {

class SessionManager {
public:
    typedef flz::RWMutex RWMutexType;

    std::shared_ptr<flz::Mutex> getSessionLock(const std::string& session_id);

private:
    RWMutexType m_mutex;
    std::map<std::string, std::weak_ptr<flz::Mutex> > m_locks;
};

typedef flz::Singleton<SessionManager> SessionManagerSingleton;

} // namespace agent

#endif // __FLZ_AGENT_SESSION_SESSION_MANAGER_H__
