#pragma once
#ifndef __FLZ_AGENT_DAO_AGENT_SESSION_DAO_H__
#define __FLZ_AGENT_DAO_AGENT_SESSION_DAO_H__

/**
 * @file    agent_session_dao.h
 * @brief   Agent 会话元数据 DAO：新建会话落库与活跃时间更新
 * @date    2026-05
 * @note    RAII: 依赖 DbGuard 管理连接
 */

#include <cstdint>
#include <memory>
#include <string>

namespace agent {

class AgentSessionDao {
public:
    typedef std::shared_ptr<AgentSessionDao> ptr;

    explicit AgentSessionDao(const std::string& db_name);

    bool exists(uint64_t user_id, const std::string& session_id);
    bool ensureSession(uint64_t user_id, const std::string& session_id,
                       const std::string& agent_id, const std::string& agent_type);

private:
    std::string m_dbName;
};

} // namespace agent

#endif // __FLZ_AGENT_DAO_AGENT_SESSION_DAO_H__
