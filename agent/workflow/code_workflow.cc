#include "agent/workflow/code_workflow.h"

#include "agent/workflow/chat_workflow.h"
#include "agent/workflow/workflow_context.h"

namespace agent {

std::string CodeWorkflow::type() const {
    return "code";
}

int CodeWorkflow::handle(WorkflowContext& ctx) {
    ctx.userMessage = "[code] " + ctx.userMessage;
    ChatWorkflow fallback;
    return fallback.handle(ctx);
}

} // namespace agent
