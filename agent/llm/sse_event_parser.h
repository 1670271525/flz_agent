#pragma once
#ifndef __FLZ_AGENT_LLM_SSE_EVENT_PARSER_H__
#define __FLZ_AGENT_LLM_SSE_EVENT_PARSER_H__

/**
 * @file    sse_event_parser.h
 * @brief   SSE 事件解析器（按 \\n\\n 分帧）
 * @date    2026-05
 * @note    RAII: 内部仅持有字符串缓冲区
 */

#include <string>
#include <vector>

namespace agent {

class SseEventParser {
public:
    std::vector<std::string> append(const std::string& chunk);

private:
    std::string m_buffer;
};

} // namespace agent

#endif // __FLZ_AGENT_LLM_SSE_EVENT_PARSER_H__
