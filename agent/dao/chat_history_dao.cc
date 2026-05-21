#include "agent/dao/chat_history_dao.h"

#include "agent/dao/db_guard.h"
#include "include/log.h"

#include <algorithm>

namespace agent {

static flz::Logger::ptr g_logger = FLZ_LOG_ROOT();

ChatHistoryDao::ChatHistoryDao(const std::string& db_name)
    : m_dbName(db_name) {
}

bool ChatHistoryDao::listRecent(uint64_t user_id, const std::string& session_id, int limit,
                                std::vector<ChatMessagePo>& out) {
    DbGuard guard(m_dbName);
    if (!guard.ok()) {
        return false;
    }

    flz::IStmt::ptr stmt = guard.db()->prepare(
        "SELECT id, agent_id, session_id, user_id, agent_name, agent_type, agent_version, "
        "agent_status, role, content, token_prompt, token_completion, ext_json "
        "FROM chat_history WHERE user_id = ? AND session_id = ? "
        "ORDER BY id DESC LIMIT ?");
    if (!stmt) {
        FLZ_LOG_ERROR(g_logger) << "prepare query chat_history fail";
        return false;
    }
    stmt->bindUint64(1, user_id);
    stmt->bindString(2, session_id);
    stmt->bindInt32(3, limit);

    flz::ISQLData::ptr res = stmt->query();
    if (!res) {
        return false;
    }

    while (res->next()) {
        ChatMessagePo po;
        po.id = res->getUint64(0);
        po.agentId = res->getString(1);
        po.sessionId = res->getString(2);
        po.userId = res->getUint64(3);
        po.agentName = res->getString(4);
        po.agentType = res->getString(5);
        po.agentVersion = res->getString(6);
        po.agentStatus = res->getInt32(7);
        po.role = res->getString(8);
        po.content = res->getString(9);
        po.tokenPrompt = res->getInt32(10);
        po.tokenCompletion = res->getInt32(11);
        po.extJson = res->getString(12);
        out.push_back(po);
    }
    std::reverse(out.begin(), out.end());
    return true;
}

bool ChatHistoryDao::saveBatch(const std::vector<ChatMessagePo>& msgs) {
    if (msgs.empty()) {
        return true;
    }
    DbGuard guard(m_dbName);
    if (!guard.ok()) {
        return false;
    }
    if (!guard.beginTransaction(false)) {
        return false;
    }

    for (size_t i = 0; i < msgs.size(); ++i) {
        const ChatMessagePo& po = msgs[i];
        flz::IStmt::ptr stmt = guard.db()->prepare(
            "INSERT INTO chat_history(agent_id, session_id, user_id, agent_name, agent_type, "
            "agent_version, agent_status, role, content, tool_name, tool_args_json, "
            "tool_result_json, token_prompt, token_completion, ext_json) "
            "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        if (!stmt) {
            guard.rollback();
            return false;
        }
        stmt->bindString(1, po.agentId);
        stmt->bindString(2, po.sessionId);
        stmt->bindUint64(3, po.userId);
        stmt->bindString(4, po.agentName);
        stmt->bindString(5, po.agentType);
        stmt->bindString(6, po.agentVersion);
        stmt->bindInt32(7, po.agentStatus);
        stmt->bindString(8, po.role);
        stmt->bindString(9, po.content);
        if (po.toolName.empty()) {
            stmt->bindNull(10);
        } else {
            stmt->bindString(10, po.toolName);
        }
        if (po.toolArgsJson.empty()) {
            stmt->bindNull(11);
        } else {
            stmt->bindString(11, po.toolArgsJson);
        }
        if (po.toolResultJson.empty()) {
            stmt->bindNull(12);
        } else {
            stmt->bindString(12, po.toolResultJson);
        }
        stmt->bindInt32(13, po.tokenPrompt);
        stmt->bindInt32(14, po.tokenCompletion);
        if (po.extJson.empty()) {
            stmt->bindNull(15);
        } else {
            stmt->bindString(15, po.extJson);
        }

        if (stmt->execute() < 0) {
            FLZ_LOG_ERROR(g_logger) << "insert chat_history fail: " << stmt->getErrStr();
            guard.rollback();
            return false;
        }
    }

    return guard.commit();
}

} // namespace agent
