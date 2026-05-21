#include "agent/workflow/chat_workflow.h"

#include "agent/dao/chat_message_po.h"
#include "agent/http/sse_writer.h"
#include "agent/workflow/workflow_context.h"
#include "agent/util/json_helper.h"

namespace agent {

std::string ChatWorkflow::type() const {
    return "chat";
}

int ChatWorkflow::handle(WorkflowContext& ctx) {
    if (!ctx.sseWriter || !ctx.chatSession || !ctx.auth) {
        return -1;
    }

    std::string answer = "收到: " + ctx.userMessage;
    for (size_t i = 0; i < answer.size(); ++i) {
        Json::Value data;
        data["delta"] = std::string(1, answer[i]);
        if (!ctx.sseWriter->sendEvent("message", JsonHelper::toString(data))) {
            return -2;
        }
    }

    if (ctx.chatDao) {
        std::vector<ChatMessagePo> batch;
        ChatMessagePo user_msg;
        user_msg.agentId = ctx.agentId;
        user_msg.sessionId = ctx.chatSession->sessionId();
        user_msg.userId = ctx.auth->userId;
        user_msg.agentType = ctx.agentType;
        user_msg.role = "user";
        user_msg.content = ctx.userMessage;
        batch.push_back(user_msg);

        ChatMessagePo assistant_msg;
        assistant_msg.agentId = ctx.agentId;
        assistant_msg.sessionId = ctx.chatSession->sessionId();
        assistant_msg.userId = ctx.auth->userId;
        assistant_msg.agentType = ctx.agentType;
        assistant_msg.role = "assistant";
        assistant_msg.content = answer;
        assistant_msg.tokenCompletion = static_cast<int>(answer.size());
        batch.push_back(assistant_msg);
        ctx.chatDao->saveBatch(batch);
    }
    return 0;
}

} // namespace agent
