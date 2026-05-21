#include "agent/http/login_servlet.h"

#include "agent/auth/jwt_helper.h"
#include "agent/config/jwt_config.h"
#include "agent/config/mysql_config.h"
#include "agent/dao/user_dao.h"
#include "agent/http/error_response.h"
#include "agent/util/json_helper.h"
#include "agent/util/time_util.h"
#include "agent/util/uuid.h"

namespace agent {

LoginServlet::LoginServlet()
    : flz::http::Servlet("LoginServlet") {
}

int32_t LoginServlet::handle(flz::http::HttpRequest::ptr request,
                             flz::http::HttpResponse::ptr response,
                             flz::http::HttpSession::ptr session) {
    (void)session;
    Json::Value body;
    if (!JsonHelper::parse(request->getBody(), body)) {
        ErrorResponse::writeJson(response, 400, "invalid json body");
        return 0;
    }

    const std::string username = body.get("username", "").asString();
    const std::string password = body.get("password", "").asString();
    if (username.empty() || password.empty()) {
        ErrorResponse::writeJson(response, 400, "username or password is empty");
        return 0;
    }

    UserDao dao(MysqlConfig::defaultDb());
    uint64_t user_id = 0;
    if (!dao.verifyPassword(username, password, user_id)) {
        ErrorResponse::writeJson(response, 401, "invalid username or password");
        return 0;
    }

    JwtConfigData jwt_cfg = JwtConfig::get();
    Json::Value claims;
    claims["user_id"] = Json::UInt64(user_id);
    claims["role"] = "user";
    claims["iss"] = jwt_cfg.issuer;
    claims["iat"] = Json::Int64(TimeUtil::nowSec());
    claims["nbf"] = Json::Int64(TimeUtil::nowSec());
    claims["exp"] = Json::Int64(TimeUtil::nowSec() + jwt_cfg.ttlSeconds);
    claims["jti"] = Uuid::generate("jti");

    std::string access_token;
    JwtHelper::encode(claims, jwt_cfg.secret, access_token);

    Json::Value refresh_claims = claims;
    refresh_claims["exp"] = Json::Int64(TimeUtil::nowSec() + jwt_cfg.refreshTtlSeconds);
    std::string refresh_token;
    JwtHelper::encode(refresh_claims, jwt_cfg.secret, refresh_token);

    Json::Value out;
    out["code"] = 0;
    out["msg"] = "ok";
    out["access_token"] = access_token;
    out["refresh_token"] = refresh_token;
    out["user_id"] = Json::UInt64(user_id);

    response->setStatus(flz::http::HttpStatus::OK);
    response->setHeader("Content-Type", "application/json; charset=utf-8");
    response->setBody(JsonHelper::toString(out));
    return 0;
}

} // namespace agent
