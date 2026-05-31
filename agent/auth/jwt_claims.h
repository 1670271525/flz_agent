#pragma once
#ifndef __FLZ_AGENT_AUTH_JWT_CLAIMS_H__
#define __FLZ_AGENT_AUTH_JWT_CLAIMS_H__

/**
 * @file    jwt_claims.h
 * @brief   JWT 校验后的 claims 结构（与 flz_chat_business 签发规范一致）
 * @date    2026-05
 * @note    RAII: 纯值对象
 */

#include <stdint.h>
#include <json/json.h>
#include <string>

namespace agent {

struct JwtClaims {
    uint64_t userId = 0;
    std::string deviceId;
    int64_t iat = 0;
    int64_t exp = 0;
    std::string iss;
    Json::Value rawPayload;
};

} // namespace agent

#endif // __FLZ_AGENT_AUTH_JWT_CLAIMS_H__
