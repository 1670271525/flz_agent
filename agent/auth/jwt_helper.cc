#include "agent/auth/jwt_helper.h"

#include "agent/util/json_helper.h"
#include "agent/util/time_util.h"
#include "include/hash_util.h"

namespace {

std::string toBase64Url(const std::string& in) {
    std::string out = flz::base64encode(in);
    for (size_t i = 0; i < out.size(); ++i) {
        if (out[i] == '+') {
            out[i] = '-';
        } else if (out[i] == '/') {
            out[i] = '_';
        }
    }
    while (!out.empty() && out[out.size() - 1] == '=') {
        out.erase(out.size() - 1);
    }
    return out;
}

std::string fromBase64Url(const std::string& in) {
    std::string out = in;
    for (size_t i = 0; i < out.size(); ++i) {
        if (out[i] == '-') {
            out[i] = '+';
        } else if (out[i] == '_') {
            out[i] = '/';
        }
    }
    while ((out.size() % 4) != 0) {
        out.push_back('=');
    }
    return flz::base64decode(out);
}

} // namespace

namespace agent {

bool JwtHelper::encode(const Json::Value& payload, const std::string& secret, std::string& token) {
    Json::Value header;
    header["alg"] = "HS256";
    header["typ"] = "JWT";

    const std::string header_b64 = toBase64Url(JsonHelper::toString(header));
    const std::string payload_b64 = toBase64Url(JsonHelper::toString(payload));
    const std::string sign_text = header_b64 + "." + payload_b64;
    const std::string signature_raw = flz::hmac_sha256(sign_text, secret);
    const std::string signature_b64 = toBase64Url(signature_raw);
    token = sign_text + "." + signature_b64;
    return true;
}

bool JwtHelper::decode(const std::string& token, const std::string& secret, Json::Value& payload) {
    std::vector<std::string> parts = flz::split(token, '.');
    if (parts.size() != 3) {
        return false;
    }

    const std::string sign_text = parts[0] + "." + parts[1];
    const std::string expect_sig = toBase64Url(flz::hmac_sha256(sign_text, secret));
    if (expect_sig != parts[2]) {
        return false;
    }

    std::string payload_text = fromBase64Url(parts[1]);
    return JsonHelper::parse(payload_text, payload);
}

bool JwtHelper::validateRegisteredClaims(const Json::Value& payload,
                                         const std::string& issuer,
                                         int clock_skew_seconds) {
    if (!payload.isObject()) {
        return false;
    }
    const int64_t now = TimeUtil::nowSec();
    if (payload.isMember("iss") && payload["iss"].asString() != issuer) {
        return false;
    }
    if (payload.isMember("nbf") && payload["nbf"].asInt64() > now + clock_skew_seconds) {
        return false;
    }
    if (payload.isMember("exp") && payload["exp"].asInt64() + clock_skew_seconds < now) {
        return false;
    }
    return true;
}

} // namespace agent
