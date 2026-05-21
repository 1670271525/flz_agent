#pragma once
#ifndef __FLZ_AGENT_DAO_DB_GUARD_H__
#define __FLZ_AGENT_DAO_DB_GUARD_H__

/**
 * @file    db_guard.h
 * @brief   MySQL 连接与事务 RAII 包装
 * @date    2026-05
 * @note    RAII: 构造获取连接，析构自动回滚未提交事务
 */

#include "db/mysql.h"

#include <memory>
#include <string>

namespace agent {

class DbGuard {
public:
    typedef std::shared_ptr<DbGuard> ptr;

    explicit DbGuard(const std::string& db_name);
    ~DbGuard();

    DbGuard(const DbGuard&) = delete;
    DbGuard& operator=(const DbGuard&) = delete;

    bool ok() const;
    flz::MySQL::ptr db() const;

    bool beginTransaction(bool auto_commit = false);
    bool commit();
    bool rollback();

private:
    flz::MySQL::ptr m_db;
    flz::ITransaction::ptr m_tx;
    bool m_committed = false;
};

} // namespace agent

#endif // __FLZ_AGENT_DAO_DB_GUARD_H__
