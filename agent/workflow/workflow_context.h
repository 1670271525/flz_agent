#pragma once
#ifndef __FLZ_AGENT_WORKFLOW_WORKFLOW_CONTEXT_H__
#define __FLZ_AGENT_WORKFLOW_WORKFLOW_CONTEXT_H__

/**
 * @file    workflow_context.h
 * @brief   工作流执行上下文
 * @date    2026-05
 * @note    RAII: 聚合会话/DAO/SSE 指针，析构时自然释放
 */

#include "agent/auth/auth_context.h"
#include "agent/dao/chat_history_dao.h"
#include "agent/http/sse_writer.h"
#include "agent/session/chat_session.h"
#include "agent/session/session_context.h"

namespace agent {

class WorkflowContext {
public:
    AuthContext::ptr auth;
    ChatSession::ptr chatSession;
    ChatHistoryDao::ptr chatDao;
    SessionContext sessionContext;
    SseWriter* sseWriter = nullptr;
    std::string agentType = "chat";
    std::string userMessage;
    std::string agentId;
};

} // namespace agent

#endif // __FLZ_AGENT_WORKFLOW_WORKFLOW_CONTEXT_H__
