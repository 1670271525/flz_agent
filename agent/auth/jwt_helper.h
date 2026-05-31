#pragma once
#ifndef __FLZ_AGENT_AUTH_JWT_HELPER_H__
#define __FLZ_AGENT_AUTH_JWT_HELPER_H__

/**
 * @file    jwt_helper.h
 * @brief   JWT HS256 校验（签发由 flz_chat_business 等外部项目完成）
 * @date    2026-05
 * @note    规范见 docs/CLIENT_API_CHAT.md §2.2
 */

#include "agent/auth/jwt_claims.h"

#include <json/json.h>
#include <string>

namespace agent {

class JwtHelper {
public:
    /// 校验 JWT 并解析 claims；失败时 err 为可读原因（与 flz_chat 一致）
    static bool verify(const std::string& token,
                       const std::string& secret,
                       const std::string& issuer,
                       int clock_skew_seconds,
                       JwtClaims& out,
                       std::string& err);

    /// 测试/联调签发（compact JSON + HS256）
    static bool encode(const Json::Value& payload, const std::string& secret, std::string& token);
};

} // namespace agent

#endif // __FLZ_AGENT_AUTH_JWT_HELPER_H__
