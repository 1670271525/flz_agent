#include "agent/http/sse_servlet.h"

#include "agent/auth/auth_filter.h"
#include "agent/config/mysql_config.h"
#include "agent/dao/chat_history_dao.h"
#include "agent/http/error_response.h"
#include "agent/http/sse_writer.h"
#include "agent/session/chat_session.h"
#include "agent/util/json_helper.h"
#include "agent/util/uuid.h"
#include "agent/workflow/workflow_context.h"
#include "agent/workflow/workflow_dispatcher.h"

namespace agent {

SseServlet::SseServlet()
    : flz::http::Servlet("SseServlet") {
}

int32_t SseServlet::handle(flz::http::HttpRequest::ptr request,
                           flz::http::HttpResponse::ptr response,
                           flz::http::HttpSession::ptr session) {
    std::string auth_err;
    AuthContext::ptr auth_ctx = AuthFilter::check(request, auth_err);
    if (!auth_ctx) {
        SseWriter writer(session);
        ErrorResponse::writeSse(writer, 401, auth_err);
        writer.close();
        response->setStatus(static_cast<flz::http::HttpStatus>(0));
        response->setBody("");
        response->setClose(true);
        return 0;
    }

    Json::Value body;
    if (!JsonHelper::parse(request->getBody(), body)) {
        SseWriter writer(session);
        ErrorResponse::writeSse(writer, 400, "invalid request body");
        writer.close();
        response->setStatus(static_cast<flz::http::HttpStatus>(0));
        response->setBody("");
        response->setClose(true);
        return 0;
    }

    const std::string session_id = body.get("session_id", "").asString();
    const std::string msg = body.get("msg", "").asString();
    const std::string agent_type = body.get("agent_type", "chat").asString();
    if (session_id.empty() || msg.empty()) {
        SseWriter writer(session);
        ErrorResponse::writeSse(writer, 400, "session_id or msg is empty");
        writer.close();
        response->setStatus(static_cast<flz::http::HttpStatus>(0));
        response->setBody("");
        response->setClose(true);
        return 0;
    }

    SseWriter writer(session);
    ChatSession::ptr chat_session(new ChatSession(session_id, auth_ctx->userId));
    ChatHistoryDao::ptr chat_dao(new ChatHistoryDao(MysqlConfig::defaultDb()));

    WorkflowContext ctx;
    ctx.auth = auth_ctx;
    ctx.chatSession = chat_session;
    ctx.chatDao = chat_dao;
    ctx.sseWriter = &writer;
    ctx.agentType = agent_type;
    ctx.userMessage = msg;
    ctx.agentId = Uuid::generate("agent");

    IWorkflow::ptr workflow = WorkflowDispatcher::GetInstance()->find(agent_type);
    if (!workflow) {
        ErrorResponse::writeSse(writer, 500, "workflow not found");
    } else if (workflow->handle(ctx) < 0) {
        ErrorResponse::writeSse(writer, 500, "workflow failed");
    } else {
        Json::Value done;
        done["finish_reason"] = "stop";
        done["usage"]["prompt_tokens"] = 0;
        done["usage"]["completion_tokens"] = 0;
        done["usage"]["total_tokens"] = 0;
        writer.sendEvent("done", JsonHelper::toString(done));
    }

    writer.close();

    // 关键：避免 HttpServer 在 servlet 后续自动追加 sendResponse 污染 SSE 流
    response->setStatus(static_cast<flz::http::HttpStatus>(0));
    response->setBody("");
    response->setClose(true);
    return 0;
}

} // namespace agent
