#pragma once
#ifndef __FLZ_AGENT_HTTP_ERROR_RESPONSE_H__
#define __FLZ_AGENT_HTTP_ERROR_RESPONSE_H__

/**
 * @file    error_response.h
 * @brief   统一错误输出工具
 * @date    2026-05
 * @note    RAII: 工具函数不持有资源
 */

#include "agent/http/sse_writer.h"
#include "http/http.h"

namespace agent {

class ErrorResponse {
public:
    static void writeJson(flz::http::HttpResponse::ptr rsp, int code, const std::string& msg);
    static void writeSse(SseWriter& writer, int code, const std::string& msg);
};

} // namespace agent

#endif // __FLZ_AGENT_HTTP_ERROR_RESPONSE_H__
