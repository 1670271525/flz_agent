#include "agent/auth/auth_filter.h"

#include "agent/auth/jwt_helper.h"
#include "agent/config/jwt_config.h"
#include "agent/util/string_util.h"

namespace agent {

AuthContext::ptr AuthFilter::check(flz::http::HttpRequest::ptr request, std::string& err_msg) {
    JwtConfigData jwt_cfg = JwtConfig::get();
    AuthContext::ptr ctx(new AuthContext());
    if (!jwt_cfg.enabled) {
        ctx->anonymous = true;
        ctx->userId = 0;
        ctx->role = "anonymous";
        return ctx;
    }

    std::string token = StringUtil::trim(request->getParam("token", ""));
    if (token.empty()) {
        for (size_t i = 0; i < jwt_cfg.headerKeys.size(); ++i) {
            std::string value = request->getHeader(jwt_cfg.headerKeys[i], "");
            if (value.empty()) {
                continue;
            }
            if (jwt_cfg.headerKeys[i] == "Authorization") {
                const std::string prefix = "Bearer ";
                if (value.find(prefix) == 0) {
                    value = value.substr(prefix.size());
                }
            }
            token = StringUtil::trim(value);
            if (!token.empty()) {
                break;
            }
        }
    }
    if (token.empty()) {
        err_msg = "missing token";
        return nullptr;
    }

    JwtClaims claims;
    if (!JwtHelper::verify(token, jwt_cfg.secret, jwt_cfg.issuer, jwt_cfg.clockSkewSeconds, claims,
                           err_msg)) {
        return nullptr;
    }

    ctx->anonymous = false;
    ctx->userId = claims.userId;
    ctx->deviceId = claims.deviceId;
    ctx->iat = claims.iat;
    ctx->exp = claims.exp;
    if (claims.rawPayload.isMember("role")) {
        ctx->role = claims.rawPayload["role"].asString();
    } else {
        ctx->role = "user";
    }
    if (claims.rawPayload.isMember("jti")) {
        ctx->jti = claims.rawPayload["jti"].asString();
    }
    return ctx;
}

} // namespace agent
