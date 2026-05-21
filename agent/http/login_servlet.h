#pragma once
#ifndef __FLZ_AGENT_HTTP_LOGIN_SERVLET_H__
#define __FLZ_AGENT_HTTP_LOGIN_SERVLET_H__

/**
 * @file    login_servlet.h
 * @brief   登录接口 /v1/auth/login
 * @date    2026-05
 * @note    RAII: Servlet 生命周期由框架托管
 */

#include "http/servlet.h"

namespace agent {

class LoginServlet : public flz::http::Servlet {
public:
    LoginServlet();
    int32_t handle(flz::http::HttpRequest::ptr request,
                   flz::http::HttpResponse::ptr response,
                   flz::http::HttpSession::ptr session) override;
};

} // namespace agent

#endif // __FLZ_AGENT_HTTP_LOGIN_SERVLET_H__
