#include "agent/config/config_loader.h"

#include "agent/config/agent_config.h"
#include "agent/config/jwt_config.h"
#include "agent/config/llm_config.h"
#include "agent/config/mcp_config.h"
#include "agent/config/mysql_config.h"
#include "include/log.h"

namespace agent {

static flz::Logger::ptr g_logger = FLZ_LOG_ROOT();

void ConfigLoader::init() {
    (void)MysqlConfig::getDbs();
    (void)LlmConfig::get();
    (void)AgentConfig::get();
    (void)McpConfig::get();
    (void)JwtConfig::get();
    FLZ_LOG_INFO(g_logger) << "ConfigLoader initialized";
}

} // namespace agent
