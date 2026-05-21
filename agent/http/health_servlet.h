#pragma once
#ifndef __FLZ_AGENT_HTTP_HEALTH_SERVLET_H__
#define __FLZ_AGENT_HTTP_HEALTH_SERVLET_H__

/**
 * @file    health_servlet.h
 * @brief   健康检查接口 /v1/health
 * @date    2026-05
 * @note    RAII: 由 Servlet 容器托管生命周期
 */

#include "http/servlet.h"

namespace agent {

class HealthServlet : public flz::http::Servlet {
public:
    HealthServlet();
    int32_t handle(flz::http::HttpRequest::ptr request,
                   flz::http::HttpResponse::ptr response,
                   flz::http::HttpSession::ptr session) override;
};

} // namespace agent

#endif // __FLZ_AGENT_HTTP_HEALTH_SERVLET_H__
