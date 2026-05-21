#include "agent/dao/user_dao.h"

#include "agent/dao/db_guard.h"

namespace agent {

UserDao::UserDao(const std::string& db_name)
    : m_dbName(db_name) {
}

bool UserDao::verifyPassword(const std::string& username, const std::string& password, uint64_t& user_id) {
    DbGuard guard(m_dbName);
    if (!guard.ok()) {
        return false;
    }

    flz::IStmt::ptr stmt = guard.db()->prepare(
        "SELECT user_id, user_password, user_status FROM user WHERE user_name = ? LIMIT 1");
    if (!stmt) {
        return false;
    }
    stmt->bindString(1, username);
    flz::ISQLData::ptr res = stmt->query();
    if (!res || !res->next()) {
        return false;
    }

    const uint64_t uid = res->getUint64(0);
    const std::string stored_pwd = res->getString(1);
    const int status = res->getInt32(2);
    if (status != 1) {
        return false;
    }

    // 兼容开发环境：当前优先做字面匹配，后续可切换 bcrypt/argon2。
    if (stored_pwd != password) {
        return false;
    }
    user_id = uid;
    return true;
}

} // namespace agent
