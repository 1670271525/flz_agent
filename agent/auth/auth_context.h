#pragma once
#ifndef __FLZ_AGENT_AUTH_AUTH_CONTEXT_H__
#define __FLZ_AGENT_AUTH_AUTH_CONTEXT_H__

/**
 * @file    auth_context.h
 * @brief   鉴权上下文定义
 * @date    2026-05
 * @note    RAII: 值对象，无外部资源占用
 */

#include <stdint.h>
#include <memory>
#include <string>

namespace agent {

/**
 * @class AuthContext
 * @brief 保存当前请求的鉴权信息
 */
class AuthContext {
public:
    typedef std::shared_ptr<AuthContext> ptr;

    uint64_t userId = 0;
    std::string deviceId;
    std::string role = "guest";
    std::string jti;
    int64_t iat = 0;
    int64_t exp = 0;
    bool anonymous = true;
};

} // namespace agent

#endif // __FLZ_AGENT_AUTH_AUTH_CONTEXT_H__
