#pragma once
#ifndef __FLZ_AGENT_MCP_TOOL_CALL_PARSER_H__
#define __FLZ_AGENT_MCP_TOOL_CALL_PARSER_H__

/**
 * @file    tool_call_parser.h
 * @brief   模型输出的 tool_calls 解析器
 * @date    2026-05
 * @note    RAII: 纯函数式解析，不持有外部资源
 */

#include <json/json.h>
#include <string>
#include <vector>

namespace agent {

struct ToolCall {
    std::string name;
    Json::Value arguments;
};

class ToolCallParser {
public:
    static bool parse(const Json::Value& llm_json, std::vector<ToolCall>& out_calls);
};

} // namespace agent

#endif // __FLZ_AGENT_MCP_TOOL_CALL_PARSER_H__
