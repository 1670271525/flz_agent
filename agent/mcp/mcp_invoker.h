#pragma once
#ifndef __FLZ_AGENT_MCP_MCP_INVOKER_H__
#define __FLZ_AGENT_MCP_MCP_INVOKER_H__

/**
 * @file    mcp_invoker.h
 * @brief   MCP 工具调用器
 * @date    2026-05
 * @note    RAII: 请求对象局部创建，生命周期由栈管理
 */

#include <json/json.h>
#include <string>

namespace agent {

class McpInvoker {
public:
    bool invoke(const std::string& tool_name, const Json::Value& args, Json::Value& result);
};

} // namespace agent

#endif // __FLZ_AGENT_MCP_MCP_INVOKER_H__
