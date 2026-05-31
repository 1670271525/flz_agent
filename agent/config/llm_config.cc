#include "agent/config/llm_config.h"

#include "agent/util/env_util.h"

#include <sstream>
#include <yaml-cpp/yaml.h>

namespace agent {

static flz::ConfigVar<LlmConfigData>::ptr g_llm_config =
    flz::Config::Lookup("llm", LlmConfigData(), "llm config");

LlmConfigData LlmConfig::get() {
    return g_llm_config->getValue();
}

LlmProviderConfig LlmConfig::getProvider(const std::string& name) {
    LlmConfigData cfg = get();
    std::map<std::string, LlmProviderConfig>::const_iterator it = cfg.providers.find(name);
    if (it != cfg.providers.end()) {
        return it->second;
    }
    std::map<std::string, LlmProviderConfig>::const_iterator def_it = cfg.providers.find(cfg.defaultProvider);
    if (def_it != cfg.providers.end()) {
        return def_it->second;
    }
    return LlmProviderConfig();
}

} // namespace agent

namespace flz {

agent::LlmProviderConfig LexicalCast<std::string, agent::LlmProviderConfig>::operator()(const std::string& v) {
    YAML::Node n = YAML::Load(v);
    agent::LlmProviderConfig out;
    out.baseUrl = n["base_url"].as<std::string>("");
    out.ssePath = n["sse_path"].as<std::string>("");
    out.apiKey = agent::EnvUtil::expand(n["api_key"].as<std::string>(""));
    out.modelId = n["model_id"].as<std::string>("");
    out.timeoutMs = n["timeout_ms"].as<uint64_t>(60000);
    out.maxRetry = n["max_retry"].as<int>(1);
    out.keepalive = n["keepalive"].as<bool>(true);
    return out;
}

std::string LexicalCast<agent::LlmProviderConfig, std::string>::operator()(const agent::LlmProviderConfig& v) {
    YAML::Node n;
    n["base_url"] = v.baseUrl;
    n["sse_path"] = v.ssePath;
    n["api_key"] = v.apiKey;
    n["model_id"] = v.modelId;
    n["timeout_ms"] = v.timeoutMs;
    n["max_retry"] = v.maxRetry;
    n["keepalive"] = v.keepalive;
    std::stringstream ss;
    ss << n;
    return ss.str();
}

agent::LlmConfigData LexicalCast<std::string, agent::LlmConfigData>::operator()(const std::string& v) {
    YAML::Node n = YAML::Load(v);
    agent::LlmConfigData out;
    out.defaultProvider = n["default"].as<std::string>("");
    if (n["providers"] && n["providers"].IsMap()) {
        for (YAML::const_iterator it = n["providers"].begin(); it != n["providers"].end(); ++it) {
            std::stringstream ss;
            ss << it->second;
            out.providers[it->first.as<std::string>()] =
                flz::LexicalCast<std::string, agent::LlmProviderConfig>()(ss.str());
        }
    }
    return out;
}

std::string LexicalCast<agent::LlmConfigData, std::string>::operator()(const agent::LlmConfigData& v) {
    YAML::Node n;
    n["default"] = v.defaultProvider;
    for (std::map<std::string, agent::LlmProviderConfig>::const_iterator it = v.providers.begin();
         it != v.providers.end(); ++it) {
        n["providers"][it->first] =
            YAML::Load(flz::LexicalCast<agent::LlmProviderConfig, std::string>()(it->second));
    }
    std::stringstream ss;
    ss << n;
    return ss.str();
}

} // namespace flz
