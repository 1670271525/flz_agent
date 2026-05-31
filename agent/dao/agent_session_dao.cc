#include "agent/dao/agent_session_dao.h"

#include "agent/dao/db_guard.h"
#include "include/log.h"

namespace agent {

static flz::Logger::ptr g_logger = FLZ_LOG_ROOT();

AgentSessionDao::AgentSessionDao(const std::string& db_name)
    : m_dbName(db_name) {
}

bool AgentSessionDao::exists(uint64_t user_id, const std::string& session_id) {
    DbGuard guard(m_dbName);
    if (!guard.ok()) {
        return false;
    }

    flz::IStmt::ptr stmt = guard.db()->prepare(
        "SELECT 1 FROM agent_session WHERE session_id = ? AND user_id = ? LIMIT 1");
    if (!stmt) {
        FLZ_LOG_ERROR(g_logger) << "prepare query agent_session fail";
        return false;
    }
    stmt->bindString(1, session_id);
    stmt->bindUint64(2, user_id);

    flz::ISQLData::ptr res = stmt->query();
    return res && res->next();
}

bool AgentSessionDao::ensureSession(uint64_t user_id, const std::string& session_id,
                                    const std::string& agent_id, const std::string& agent_type) {
    DbGuard guard(m_dbName);
    if (!guard.ok()) {
        return false;
    }

    flz::IStmt::ptr stmt = guard.db()->prepare(
        "INSERT INTO agent_session(session_id, user_id, agent_id, agent_type, last_active_at) "
        "VALUES(?, ?, ?, ?, NOW()) "
        "ON DUPLICATE KEY UPDATE last_active_at = NOW()");
    if (!stmt) {
        FLZ_LOG_ERROR(g_logger) << "prepare upsert agent_session fail";
        return false;
    }
    stmt->bindString(1, session_id);
    stmt->bindUint64(2, user_id);
    stmt->bindString(3, agent_id);
    stmt->bindString(4, agent_type);

    if (stmt->execute() < 0) {
        FLZ_LOG_ERROR(g_logger) << "upsert agent_session fail: " << stmt->getErrStr()
                                << " session_id=" << session_id << " user_id=" << user_id;
        return false;
    }
    return true;
}

} // namespace agent
