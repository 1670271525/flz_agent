#include "agent/config/config_loader.h"

#include "agent/config/agent_config.h"
#include "agent/config/jwt_config.h"
#include "agent/config/llm_config.h"
#include "agent/config/mcp_config.h"
#include "agent/config/mysql_config.h"
#include "agent/util/env_util.h"
#include "include/log.h"

namespace agent {

static flz::Logger::ptr g_logger = FLZ_LOG_ROOT();

void ConfigLoader::init() {
    (void)MysqlConfig::getDbs();
    LlmConfigData llm_cfg = LlmConfig::get();
    (void)AgentConfig::get();
    (void)McpConfig::get();
    (void)JwtConfig::get();

    const LlmProviderConfig def_llm = LlmConfig::getProvider("");
    if (def_llm.apiKey.empty()) {
        FLZ_LOG_WARN(g_logger) << "LLM default provider api_key is empty (provider="
                               << llm_cfg.defaultProvider << ")";
        FLZ_LOG_WARN(g_logger) << "llm.yml uses ${DEEPSEEK_API_KEY}; export DEEPSEEK_API_KEY=sk-xxx before start"
                               << " (jwt.yml secret != DeepSeek API key)";
    }

    FLZ_LOG_INFO(g_logger) << "ConfigLoader initialized";
}

} // namespace agent
