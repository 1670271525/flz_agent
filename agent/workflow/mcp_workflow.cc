#include "agent/workflow/mcp_workflow.h"

#include "agent/workflow/chat_workflow.h"
#include "agent/workflow/workflow_context.h"

namespace agent {

std::string McpWorkflow::type() const {
    return "mcp";
}

int McpWorkflow::handle(WorkflowContext& ctx) {
    ctx.userMessage = "[mcp] " + ctx.userMessage;
    ChatWorkflow fallback;
    return fallback.handle(ctx);
}

} // namespace agent
