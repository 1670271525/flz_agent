#include "agent/module/agent_module.h"

#include "agent/config/config_loader.h"
#include "agent/http/health_servlet.h"
#include "agent/http/sse_servlet.h"
#include "agent/workflow/chat_workflow.h"
#include "agent/workflow/code_workflow.h"
#include "agent/workflow/draw_workflow.h"
#include "agent/workflow/mcp_workflow.h"
#include "agent/workflow/workflow_dispatcher.h"
#include "include/application.h"
#include "include/log.h"

namespace agent {

static flz::Logger::ptr g_logger = FLZ_LOG_ROOT();

AgentModule::AgentModule()
    : flz::Module("flz_agent", "1.0", "") {
}

bool AgentModule::onLoad() {
    FLZ_LOG_INFO(g_logger) << "AgentModule::onLoad";
    ConfigLoader::init();
    WorkflowDispatcher::GetInstance()->registerWorkflow(std::make_shared<ChatWorkflow>());
    WorkflowDispatcher::GetInstance()->registerWorkflow(std::make_shared<CodeWorkflow>());
    WorkflowDispatcher::GetInstance()->registerWorkflow(std::make_shared<DrawWorkflow>());
    WorkflowDispatcher::GetInstance()->registerWorkflow(std::make_shared<McpWorkflow>());
    return true;
}

bool AgentModule::onUnload() {
    FLZ_LOG_INFO(g_logger) << "AgentModule::onUnload";
    return true;
}

bool AgentModule::onServerReady() {
    FLZ_LOG_INFO(g_logger) << "AgentModule::onServerReady";
    std::vector<flz::TcpServer::ptr> servers;
    if (!flz::Application::GetInstance()->getServer("http", servers)) {
        FLZ_LOG_WARN(g_logger) << "no http server found";
        return true;
    }

    for (size_t i = 0; i < servers.size(); ++i) {
        flz::http::HttpServer::ptr http_server =
            std::dynamic_pointer_cast<flz::http::HttpServer>(servers[i]);
        if (!http_server) {
            continue;
        }
        flz::http::ServletDispatch::ptr dispatch = http_server->getServletDispatch();
        dispatch->addServlet("/v1/chat/sse", std::make_shared<SseServlet>());
        dispatch->addServlet("/v1/health", std::make_shared<HealthServlet>());
    }
    return true;
}

bool AgentModule::onServerUp() {
    FLZ_LOG_INFO(g_logger) << "AgentModule::onServerUp";
    return true;
}

} // namespace agent

extern "C" {

flz::Module* CreateModule() {
    flz::Module* module = new agent::AgentModule();
    FLZ_LOG_INFO(agent::g_logger) << "CreateModule " << module;
    return module;
}

void DestoryModule(flz::Module* module) {
    FLZ_LOG_INFO(agent::g_logger) << "DestoryModule " << module;
    delete module;
}

}
