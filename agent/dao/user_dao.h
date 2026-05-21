#pragma once
#ifndef __FLZ_AGENT_DAO_USER_DAO_H__
#define __FLZ_AGENT_DAO_USER_DAO_H__

/**
 * @file    user_dao.h
 * @brief   用户查询与密码校验 DAO
 * @date    2026-05
 * @note    RAII: 依赖 MySQLStmt 自动回收 DB 资源
 */

#include <memory>
#include <string>

namespace agent {

class UserDao {
public:
    typedef std::shared_ptr<UserDao> ptr;

    explicit UserDao(const std::string& db_name);
    bool verifyPassword(const std::string& username, const std::string& password, uint64_t& user_id);

private:
    std::string m_dbName;
};

} // namespace agent

#endif // __FLZ_AGENT_DAO_USER_DAO_H__
