#include "agent/mcp/mcp_registry.h"

#include "agent/config/mcp_config.h"
#include "agent/util/json_helper.h"

namespace agent {

McpRegistry* McpRegistry::GetInstance() {
    return McpRegistrySingleton::GetInstance();
}

void McpRegistry::reloadFromConfig() {
    std::vector<McpToolDescriptor> tmp;
    McpConfigData cfg = McpConfig::get();
    for (size_t i = 0; i < cfg.services.size(); ++i) {
        const McpServiceConfig& service = cfg.services[i];
        for (size_t j = 0; j < service.tools.size(); ++j) {
            McpToolDescriptor d;
            d.serviceName = service.name;
            d.transport = service.transport;
            d.endpoint = service.endpoint;
            d.timeoutMs = service.timeoutMs;
            d.name = service.tools[j].name;
            d.description = service.tools[j].description;
            d.parameters = service.tools[j].parameters;
            tmp.push_back(d);
        }
    }

    RWMutexType::WriteLock lock(m_mutex);
    m_tools.swap(tmp);
}

std::vector<McpToolDescriptor> McpRegistry::listTools() const {
    RWMutexType::ReadLock lock(m_mutex);
    return m_tools;
}

bool McpRegistry::findTool(const std::string& tool_name, McpToolDescriptor& out) const {
    RWMutexType::ReadLock lock(m_mutex);
    for (size_t i = 0; i < m_tools.size(); ++i) {
        if (m_tools[i].name == tool_name) {
            out = m_tools[i];
            return true;
        }
    }
    return false;
}

std::string McpRegistry::dumpToolsAsJson() const {
    Json::Value root(Json::arrayValue);
    std::vector<McpToolDescriptor> tools = listTools();
    for (size_t i = 0; i < tools.size(); ++i) {
        Json::Value t;
        t["service"] = tools[i].serviceName;
        t["transport"] = tools[i].transport;
        t["endpoint"] = tools[i].endpoint;
        t["name"] = tools[i].name;
        t["description"] = tools[i].description;
        t["parameters"] = tools[i].parameters;
        root.append(t);
    }
    return JsonHelper::toString(root);
}

} // namespace agent
