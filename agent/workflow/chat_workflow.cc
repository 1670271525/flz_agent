#include "agent/workflow/chat_workflow.h"

#include "agent/config/agent_config.h"
#include "agent/dao/chat_message_po.h"
#include "agent/http/sse_writer.h"
#include "agent/llm/llm_factory.h"
#include "agent/llm/prompt_builder.h"
#include "agent/mcp/mcp_invoker.h"
#include "agent/mcp/mcp_registry.h"
#include "agent/mcp/tool_call_parser.h"
#include "agent/util/json_helper.h"
#include "agent/workflow/workflow_context.h"
#include "include/log.h"

#include <algorithm>
#include <vector>

namespace agent {

static flz::Logger::ptr g_logger = FLZ_LOG_ROOT();

namespace {

static ChatMessagePo buildBasePo(const WorkflowContext& ctx) {
    ChatMessagePo po;
    po.agentId = ctx.agentId;
    po.sessionId = ctx.chatSession->sessionId();
    po.userId = ctx.auth->userId;
    po.agentType = ctx.agentType;
    return po;
}

static void applyUsage(WorkflowContext& ctx, const Json::Value& usage) {
    if (!usage.isObject()) {
        return;
    }
    ctx.promptTokens += usage.get("prompt_tokens", 0).asInt();
    ctx.completionTokens += usage.get("completion_tokens", 0).asInt();
    ctx.totalTokens += usage.get("total_tokens", 0).asInt();
}

} // namespace

std::string ChatWorkflow::type() const {
    return "chat";
}

int ChatWorkflow::handle(WorkflowContext& ctx) {
    if (!ctx.sseWriter || !ctx.chatSession || !ctx.auth) {
        return -1;
    }

    AgentConfigData cfg = AgentConfig::get();
    const int max_llm_calls = std::max(1, cfg.invocation.maxLlmCallsPerRequest);
    const int max_tool_calls = std::max(0, cfg.invocation.maxToolCallsPerRequest);

    std::vector<ChatMessagePo> runtime_history = ctx.sessionContext.messages();
    std::vector<ChatMessagePo> save_batch;

    ChatMessagePo user_msg = buildBasePo(ctx);
    user_msg.role = "user";
    user_msg.content = ctx.userMessage;
    user_msg.tokenPrompt = 0;
    user_msg.tokenCompletion = 0;
    runtime_history.push_back(user_msg);
    save_batch.push_back(user_msg);

    const bool enable_tools = (ctx.agentType == "mcp");
    const std::string tool_schema_json =
        enable_tools ? McpRegistry::GetInstance()->dumpToolsAsJson() : "[]";
    ILlmClient::ptr llm_client = LlmFactory::create("");
    if (!llm_client) {
        FLZ_LOG_ERROR(g_logger) << "llm client create failed";
        return -2;
    }

    McpInvoker mcp_invoker;
    int tool_call_count = 0;
    bool finished = false;
    for (int round = 0; round < max_llm_calls; ++round) {
        const std::vector<LlmMessage> llm_messages =
            PromptBuilder::build(runtime_history, ctx.agentType, tool_schema_json);
        std::string assistant_text;
        Json::Value llm_result;
        int rt = llm_client->streamChat(
            llm_messages,
            [&](const std::string& delta) -> bool {
                assistant_text.append(delta);
                Json::Value sse_data;
                sse_data["delta"] = delta;
                return ctx.sseWriter->sendEvent("message", JsonHelper::toString(sse_data));
            },
            &llm_result);
        if (rt < 0) {
            FLZ_LOG_ERROR(g_logger) << "llm stream failed, round=" << round << " rt=" << rt;
            return -3;
        }

        const Json::Value message = llm_result["message"];
        if (assistant_text.empty()) {
            assistant_text = message.get("content", "").asString();
        }
        ChatMessagePo assistant_msg = buildBasePo(ctx);
        assistant_msg.role = "assistant";
        assistant_msg.content = assistant_text;
        const Json::Value usage = llm_result["usage"];
        assistant_msg.tokenPrompt = usage.get("prompt_tokens", 0).asInt();
        assistant_msg.tokenCompletion = usage.get("completion_tokens", 0).asInt();
        if (message.isMember("tool_calls")) {
            assistant_msg.toolArgsJson = JsonHelper::toString(message["tool_calls"]);
        }
        runtime_history.push_back(assistant_msg);
        save_batch.push_back(assistant_msg);
        ctx.finalAnswer = assistant_text;

        if (llm_result.isMember("finish_reason")) {
            ctx.finishReason = llm_result["finish_reason"].asString();
        }
        applyUsage(ctx, usage);

        std::vector<ToolCall> tool_calls;
        if (!ToolCallParser::parse(llm_result, tool_calls)) {
            return -4;
        }
        if (!enable_tools || tool_calls.empty()) {
            finished = true;
            break;
        }

        for (size_t i = 0; i < tool_calls.size(); ++i) {
            if (tool_call_count >= max_tool_calls) {
                FLZ_LOG_WARN(g_logger) << "tool call exceeds limit, max=" << max_tool_calls;
                return -5;
            }
            ++tool_call_count;

            Json::Value tool_call_evt;
            tool_call_evt["name"] = tool_calls[i].name;
            tool_call_evt["args"] = tool_calls[i].arguments;
            if (!ctx.sseWriter->sendEvent("tool_call", JsonHelper::toString(tool_call_evt))) {
                return -6;
            }

            Json::Value tool_result;
            bool ok = mcp_invoker.invoke(tool_calls[i].name, tool_calls[i].arguments, tool_result);
            if (!ctx.sseWriter->sendEvent("tool_result", JsonHelper::toString(tool_result))) {
                return -7;
            }

            ChatMessagePo tool_msg = buildBasePo(ctx);
            tool_msg.role = "tool";
            tool_msg.content = JsonHelper::toString(tool_result);
            tool_msg.toolName = tool_calls[i].name;
            tool_msg.toolArgsJson = JsonHelper::toString(tool_calls[i].arguments);
            tool_msg.toolResultJson = tool_msg.content;
            runtime_history.push_back(tool_msg);
            save_batch.push_back(tool_msg);

            if (!ok) {
                FLZ_LOG_WARN(g_logger) << "invoke tool failed, name=" << tool_calls[i].name;
            }
        }
    }

    if (!finished) {
        FLZ_LOG_WARN(g_logger) << "llm call exceeds limit, max=" << max_llm_calls;
        return -8;
    }

    if (ctx.chatDao && !ctx.chatDao->saveBatch(save_batch)) {
        FLZ_LOG_ERROR(g_logger) << "chat_history saveBatch failed, size=" << save_batch.size();
        return -9;
    }
    return 0;
}

} // namespace agent
