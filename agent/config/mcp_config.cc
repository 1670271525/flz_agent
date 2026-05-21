#include "agent/config/mcp_config.h"

#include "agent/util/json_helper.h"

#include <sstream>
#include <yaml-cpp/yaml.h>

namespace agent {

static flz::ConfigVar<McpConfigData>::ptr g_mcp_config =
    flz::Config::Lookup("mcp", McpConfigData(), "mcp config");

McpConfigData McpConfig::get() {
    return g_mcp_config->getValue();
}

} // namespace agent

namespace flz {

agent::McpConfigData LexicalCast<std::string, agent::McpConfigData>::operator()(const std::string& v) {
    YAML::Node n = YAML::Load(v);
    agent::McpConfigData out;
    if (!n["services"] || !n["services"].IsSequence()) {
        return out;
    }

    for (size_t i = 0; i < n["services"].size(); ++i) {
        agent::McpServiceConfig service;
        YAML::Node sn = n["services"][i];
        service.name = sn["name"].as<std::string>("");
        service.description = sn["description"].as<std::string>("");
        service.transport = sn["transport"].as<std::string>("http");
        service.endpoint = sn["endpoint"].as<std::string>("");
        service.timeoutMs = sn["timeout_ms"].as<int>(5000);

        if (sn["tools"] && sn["tools"].IsSequence()) {
            for (size_t j = 0; j < sn["tools"].size(); ++j) {
                agent::McpToolConfig tool;
                YAML::Node tn = sn["tools"][j];
                tool.name = tn["name"].as<std::string>("");
                tool.description = tn["description"].as<std::string>("");
                std::stringstream ss;
                ss << tn["parameters"];
                agent::JsonHelper::parse(ss.str(), tool.parameters);
                service.tools.push_back(tool);
            }
        }
        out.services.push_back(service);
    }
    return out;
}

std::string LexicalCast<agent::McpConfigData, std::string>::operator()(const agent::McpConfigData& v) {
    YAML::Node n;
    for (size_t i = 0; i < v.services.size(); ++i) {
        const agent::McpServiceConfig& service = v.services[i];
        YAML::Node sn;
        sn["name"] = service.name;
        sn["description"] = service.description;
        sn["transport"] = service.transport;
        sn["endpoint"] = service.endpoint;
        sn["timeout_ms"] = service.timeoutMs;

        for (size_t j = 0; j < service.tools.size(); ++j) {
            const agent::McpToolConfig& tool = service.tools[j];
            YAML::Node tn;
            tn["name"] = tool.name;
            tn["description"] = tool.description;
            tn["parameters"] = YAML::Load(agent::JsonHelper::toString(tool.parameters));
            sn["tools"].push_back(tn);
        }
        n["services"].push_back(sn);
    }

    std::stringstream ss;
    ss << n;
    return ss.str();
}

} // namespace flz
