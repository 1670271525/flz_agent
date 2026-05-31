#pragma once
#ifndef __FLZ_AGENT_MCP_MCP_DESCRIPTOR_H__
#define __FLZ_AGENT_MCP_MCP_DESCRIPTOR_H__

/**
 * @file    mcp_descriptor.h
 * @brief   MCP 工具描述结构
 * @date    2026-05
 * @note    RAII: 值对象，不持有外部资源
 */

#include <json/json.h>
#include <string>
#include <vector>

namespace agent {

struct McpToolDescriptor {
    std::string serviceName;
    std::string transport;
    std::string endpoint;
    int timeoutMs = 5000;
    std::string name;
    std::string description;
    Json::Value parameters;
};

} // namespace agent

#endif // __FLZ_AGENT_MCP_MCP_DESCRIPTOR_H__
