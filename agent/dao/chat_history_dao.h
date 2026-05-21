#pragma once
#ifndef __FLZ_AGENT_DAO_CHAT_HISTORY_DAO_H__
#define __FLZ_AGENT_DAO_CHAT_HISTORY_DAO_H__

/**
 * @file    chat_history_dao.h
 * @brief   聊天记录 DAO：查询上下文与批量落库
 * @date    2026-05
 * @note    RAII: 依赖 DbGuard 与 MySQLStmt 自动管理资源
 */

#include "agent/dao/chat_message_po.h"

#include <memory>
#include <string>
#include <vector>

namespace agent {

class ChatHistoryDao {
public:
    typedef std::shared_ptr<ChatHistoryDao> ptr;

    explicit ChatHistoryDao(const std::string& db_name);

    bool listRecent(uint64_t user_id, const std::string& session_id, int limit, std::vector<ChatMessagePo>& out);
    bool saveBatch(const std::vector<ChatMessagePo>& msgs);

private:
    std::string m_dbName;
};

} // namespace agent

#endif // __FLZ_AGENT_DAO_CHAT_HISTORY_DAO_H__
