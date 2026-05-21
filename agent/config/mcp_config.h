#pragma once
#ifndef __FLZ_AGENT_CONFIG_MCP_CONFIG_H__
#define __FLZ_AGENT_CONFIG_MCP_CONFIG_H__

/**
 * @file    mcp_config.h
 * @brief   MCP 服务配置定义
 * @date    2026-05
 * @note    RAII: 配置为值对象，不直接持有连接
 */

#include "include/config.h"

#include <json/json.h>
#include <map>
#include <string>
#include <vector>

namespace agent {

struct McpToolConfig {
    std::string name;
    std::string description;
    Json::Value parameters;

    bool operator==(const McpToolConfig& other) const {
        return name == other.name
            && description == other.description
            && parameters.toStyledString() == other.parameters.toStyledString();
    }
};

struct McpServiceConfig {
    std::string name;
    std::string description;
    std::string transport;
    std::string endpoint;
    int timeoutMs = 5000;
    std::vector<McpToolConfig> tools;

    bool operator==(const McpServiceConfig& other) const {
        return name == other.name
            && description == other.description
            && transport == other.transport
            && endpoint == other.endpoint
            && timeoutMs == other.timeoutMs
            && tools == other.tools;
    }
};

struct McpConfigData {
    std::vector<McpServiceConfig> services;

    bool operator==(const McpConfigData& other) const {
        return services == other.services;
    }
};

class McpConfig {
public:
    static McpConfigData get();
};

} // namespace agent

namespace flz {

template<>
class LexicalCast<std::string, agent::McpConfigData> {
public:
    agent::McpConfigData operator()(const std::string& v);
};

template<>
class LexicalCast<agent::McpConfigData, std::string> {
public:
    std::string operator()(const agent::McpConfigData& v);
};

} // namespace flz

#endif // __FLZ_AGENT_CONFIG_MCP_CONFIG_H__
