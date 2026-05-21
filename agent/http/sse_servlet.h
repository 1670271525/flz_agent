#pragma once
#ifndef __FLZ_AGENT_HTTP_SSE_SERVLET_H__
#define __FLZ_AGENT_HTTP_SSE_SERVLET_H__

/**
 * @file    sse_servlet.h
 * @brief   SSE 接口 /v1/chat/sse
 * @date    2026-05
 * @note    RAII: 使用 SseWriter 在析构阶段回收连接
 */

#include "http/servlet.h"

namespace agent {

class SseServlet : public flz::http::Servlet {
public:
    SseServlet();
    int32_t handle(flz::http::HttpRequest::ptr request,
                   flz::http::HttpResponse::ptr response,
                   flz::http::HttpSession::ptr session) override;
};

} // namespace agent

#endif // __FLZ_AGENT_HTTP_SSE_SERVLET_H__
