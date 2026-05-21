#include "agent/config/agent_config.h"

#include <sstream>
#include <yaml-cpp/yaml.h>

namespace agent {

static flz::ConfigVar<AgentConfigData>::ptr g_agent_config =
    flz::Config::Lookup("agent", AgentConfigData(), "agent config");

AgentConfigData AgentConfig::get() {
    return g_agent_config->getValue();
}

} // namespace agent

namespace flz {

agent::AgentConfigData LexicalCast<std::string, agent::AgentConfigData>::operator()(const std::string& v) {
    YAML::Node n = YAML::Load(v);
    agent::AgentConfigData out;
    out.context.maxRounds = n["context"]["max_rounds"].as<int>(10);
    out.context.maxTotalTokens = n["context"]["max_total_tokens"].as<int>(8000);
    out.context.maxMessageChars = n["context"]["max_message_chars"].as<int>(8000);

    out.invocation.maxToolCallsPerRequest =
        n["invocation"]["max_tool_calls_per_request"].as<int>(8);
    out.invocation.maxLlmCallsPerRequest =
        n["invocation"]["max_llm_calls_per_request"].as<int>(6);
    out.invocation.requestTimeoutMs =
        n["invocation"]["request_timeout_ms"].as<int>(120000);

    out.workflows.defaultType = n["workflows"]["default"].as<std::string>("chat");
    if (n["workflows"]["enabled"] && n["workflows"]["enabled"].IsSequence()) {
        out.workflows.enabled = n["workflows"]["enabled"].as<std::vector<std::string> >();
    }
    return out;
}

std::string LexicalCast<agent::AgentConfigData, std::string>::operator()(const agent::AgentConfigData& v) {
    YAML::Node n;
    n["context"]["max_rounds"] = v.context.maxRounds;
    n["context"]["max_total_tokens"] = v.context.maxTotalTokens;
    n["context"]["max_message_chars"] = v.context.maxMessageChars;
    n["invocation"]["max_tool_calls_per_request"] = v.invocation.maxToolCallsPerRequest;
    n["invocation"]["max_llm_calls_per_request"] = v.invocation.maxLlmCallsPerRequest;
    n["invocation"]["request_timeout_ms"] = v.invocation.requestTimeoutMs;
    n["workflows"]["default"] = v.workflows.defaultType;
    n["workflows"]["enabled"] = v.workflows.enabled;

    std::stringstream ss;
    ss << n;
    return ss.str();
}

} // namespace flz
