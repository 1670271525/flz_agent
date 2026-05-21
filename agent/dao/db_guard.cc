#include "agent/dao/db_guard.h"

#include "include/log.h"

namespace agent {

static flz::Logger::ptr g_logger = FLZ_LOG_ROOT();

DbGuard::DbGuard(const std::string& db_name) {
    m_db = flz::MySQLMgr::GetInstance()->get(db_name);
    if (!m_db) {
        FLZ_LOG_ERROR(g_logger) << "DbGuard get mysql fail, db=" << db_name;
    } else if (!m_db->ping()) {
        FLZ_LOG_WARN(g_logger) << "DbGuard ping mysql fail, db=" << db_name;
    }
}

DbGuard::~DbGuard() {
    if (m_tx && !m_committed) {
        m_tx->rollback();
    }
}

bool DbGuard::ok() const {
    return (bool)m_db;
}

flz::MySQL::ptr DbGuard::db() const {
    return m_db;
}

bool DbGuard::beginTransaction(bool auto_commit) {
    if (!m_db) {
        return false;
    }
    m_tx = m_db->openTransaction(auto_commit);
    return (bool)m_tx;
}

bool DbGuard::commit() {
    if (!m_tx) {
        return true;
    }
    m_committed = m_tx->commit();
    return m_committed;
}

bool DbGuard::rollback() {
    if (!m_tx) {
        return true;
    }
    return m_tx->rollback();
}

} // namespace agent
