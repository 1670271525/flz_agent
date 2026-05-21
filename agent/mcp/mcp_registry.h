#pragma once
#ifndef __FLZ_AGENT_MCP_MCP_REGISTRY_H__
#define __FLZ_AGENT_MCP_MCP_REGISTRY_H__

/**
 * @file    mcp_registry.h
 * @brief   MCP 工具注册中心
 * @date    2026-05
 * @note    RAII: 单例内数据由容器自动释放，RWMutex 保护并发访问
 */

#include "agent/mcp/mcp_descriptor.h"
#include "include/mutex.h"
#include "include/singleton.h"

#include <vector>

namespace agent {

class McpRegistry {
public:
    typedef flz::RWMutex RWMutexType;
    static McpRegistry* GetInstance();

    void reloadFromConfig();
    std::vector<McpToolDescriptor> listTools() const;
    std::string dumpToolsAsJson() const;

private:
    mutable RWMutexType m_mutex;
    std::vector<McpToolDescriptor> m_tools;
};

typedef flz::Singleton<McpRegistry> McpRegistrySingleton;

} // namespace agent

#endif // __FLZ_AGENT_MCP_MCP_REGISTRY_H__
