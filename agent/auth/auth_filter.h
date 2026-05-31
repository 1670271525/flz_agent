#pragma once
#ifndef __FLZ_AGENT_AUTH_AUTH_FILTER_H__
#define __FLZ_AGENT_AUTH_AUTH_FILTER_H__

/**
 * @file    auth_filter.h
 * @brief   Servlet 鉴权前置过滤器（仅校验 JWT，不负责登录签发）
 * @date    2026-05
 * @note    RAII: 仅处理请求数据，不持久占用资源
 */

#include "agent/auth/auth_context.h"

#include "http/http.h"

namespace agent {

class AuthFilter {
public:
    static AuthContext::ptr check(flz::http::HttpRequest::ptr request, std::string& err_msg);
};

} // namespace agent

#endif // __FLZ_AGENT_AUTH_AUTH_FILTER_H__
