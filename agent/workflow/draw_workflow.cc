#include "agent/workflow/draw_workflow.h"

#include "agent/workflow/chat_workflow.h"
#include "agent/workflow/workflow_context.h"

namespace agent {

std::string DrawWorkflow::type() const {
    return "draw";
}

int DrawWorkflow::handle(WorkflowContext& ctx) {
    ctx.userMessage = "[draw] " + ctx.userMessage;
    ChatWorkflow fallback;
    return fallback.handle(ctx);
}

} // namespace agent
