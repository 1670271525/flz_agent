#include "agent/config/jwt_config.h"

#include "agent/util/env_util.h"

#include <sstream>
#include <yaml-cpp/yaml.h>

namespace agent {

static flz::ConfigVar<JwtConfigData>::ptr g_jwt_config =
    flz::Config::Lookup("jwt", JwtConfigData(), "jwt config");

JwtConfigData JwtConfig::get() {
    return g_jwt_config->getValue();
}

} // namespace agent

namespace flz {

agent::JwtConfigData LexicalCast<std::string, agent::JwtConfigData>::operator()(const std::string& v) {
    YAML::Node n = YAML::Load(v);
    agent::JwtConfigData out;
    out.enabled = n["enabled"].as<bool>(true);
    out.algorithm = n["algorithm"].as<std::string>("HS256");
    out.secret = agent::EnvUtil::expand(n["secret"].as<std::string>(""));
    out.issuer = n["issuer"].as<std::string>("flz_chat_business");
    out.ttlSeconds = n["ttl_seconds"].as<int>(7200);
    out.refreshTtlSeconds = n["refresh_ttl_seconds"].as<int>(1209600);
    out.clockSkewSeconds = n["clock_skew_seconds"].as<int>(60);
    if (n["header_keys"] && n["header_keys"].IsSequence()) {
        out.headerKeys = n["header_keys"].as<std::vector<std::string> >();
    } else {
        out.headerKeys.push_back("Authorization");
        out.headerKeys.push_back("X-Token");
    }
    return out;
}

std::string LexicalCast<agent::JwtConfigData, std::string>::operator()(const agent::JwtConfigData& v) {
    YAML::Node n;
    n["enabled"] = v.enabled;
    n["algorithm"] = v.algorithm;
    n["secret"] = v.secret;
    n["issuer"] = v.issuer;
    n["ttl_seconds"] = v.ttlSeconds;
    n["refresh_ttl_seconds"] = v.refreshTtlSeconds;
    n["clock_skew_seconds"] = v.clockSkewSeconds;
    n["header_keys"] = v.headerKeys;
    std::stringstream ss;
    ss << n;
    return ss.str();
}

} // namespace flz
