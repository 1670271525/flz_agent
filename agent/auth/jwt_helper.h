#pragma once
#ifndef __FLZ_AGENT_AUTH_JWT_HELPER_H__
#define __FLZ_AGENT_AUTH_JWT_HELPER_H__

/**
 * @file    jwt_helper.h
 * @brief   JWT HS256 编解码与校验
 * @date    2026-05
 * @note    RAII: 算法函数不持有长期资源
 */

#include <json/json.h>
#include <string>

namespace agent {

class JwtHelper {
public:
    static bool encode(const Json::Value& payload, const std::string& secret, std::string& token);
    static bool decode(const std::string& token, const std::string& secret, Json::Value& payload);
    static bool validateRegisteredClaims(const Json::Value& payload,
                                         const std::string& issuer,
                                         int clock_skew_seconds);
};

} // namespace agent

#endif // __FLZ_AGENT_AUTH_JWT_HELPER_H__
