#pragma once
#ifndef __FLZ_AGENT_CONFIG_JWT_CONFIG_H__
#define __FLZ_AGENT_CONFIG_JWT_CONFIG_H__

/**
 * @file    jwt_config.h
 * @brief   JWT 配置定义与读取
 * @date    2026-05
 * @note    RAII: 配置为纯值对象，不持有外部句柄
 */

#include "include/config.h"

#include <string>
#include <vector>

namespace agent {

struct JwtConfigData {
    bool enabled = true;
    std::string algorithm = "HS256";
    std::string secret;
    std::string issuer = "flz_agent";
    int ttlSeconds = 7200;
    int refreshTtlSeconds = 1209600;
    int clockSkewSeconds = 30;
    std::vector<std::string> headerKeys;

    bool operator==(const JwtConfigData& other) const {
        return enabled == other.enabled
            && algorithm == other.algorithm
            && secret == other.secret
            && issuer == other.issuer
            && ttlSeconds == other.ttlSeconds
            && refreshTtlSeconds == other.refreshTtlSeconds
            && clockSkewSeconds == other.clockSkewSeconds
            && headerKeys == other.headerKeys;
    }
};

class JwtConfig {
public:
    static JwtConfigData get();
};

} // namespace agent

namespace flz {

template<>
class LexicalCast<std::string, agent::JwtConfigData> {
public:
    agent::JwtConfigData operator()(const std::string& v);
};

template<>
class LexicalCast<agent::JwtConfigData, std::string> {
public:
    std::string operator()(const agent::JwtConfigData& v);
};

} // namespace flz

#endif // __FLZ_AGENT_CONFIG_JWT_CONFIG_H__
