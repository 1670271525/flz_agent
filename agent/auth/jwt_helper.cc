#include "agent/auth/jwt_helper.h"

#include "agent/util/json_helper.h"
#include "include/hash_util.h"

#include <cstdlib>
#include <ctime>
#include <openssl/crypto.h>
#include <openssl/hmac.h>
#include <vector>

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

bool fromBase64Url(const std::string& in, std::string& out) {
    if (in.empty()) {
        return false;
    }
    std::string base = in;
    for (size_t i = 0; i < base.size(); ++i) {
        if (base[i] == '-') {
            base[i] = '+';
        } else if (base[i] == '_') {
            base[i] = '/';
        }
    }
    while (base.size() % 4 != 0) {
        base.push_back('=');
    }
    out = flz::base64decode(base);
    return true;
}

bool parseUInt64(const std::string& in, uint64_t& out) {
    if (in.empty()) {
        return false;
    }
    char* end = nullptr;
    const unsigned long long v = strtoull(in.c_str(), &end, 10);
    if (end == nullptr || *end != '\0') {
        return false;
    }
    out = static_cast<uint64_t>(v);
    return true;
}

} // namespace

namespace agent {

bool JwtHelper::verify(const std::string& token,
                       const std::string& secret,
                       const std::string& issuer,
                       int clock_skew_seconds,
                       JwtClaims& out,
                       std::string& err) {
    out = JwtClaims();

    const std::vector<std::string> parts = flz::split(token, '.');
    if (parts.size() != 3 || parts[0].empty() || parts[1].empty() || parts[2].empty()) {
        err = "token format invalid";
        return false;
    }

    std::string header_json;
    std::string payload_json;
    if (!fromBase64Url(parts[0], header_json) || !fromBase64Url(parts[1], payload_json)) {
        err = "token base64 decode failed";
        return false;
    }

    Json::Value header;
    Json::Value payload;
    if (!JsonHelper::parse(header_json, header) || !header.isObject()) {
        err = "token header invalid";
        return false;
    }
    if (!JsonHelper::parse(payload_json, payload) || !payload.isObject()) {
        err = "token payload invalid";
        return false;
    }

    const std::string alg = header.get("alg", "").asString();
    if (alg != "HS256") {
        err = "token alg invalid";
        return false;
    }

    const std::string signing_input = parts[0] + "." + parts[1];
    unsigned int digest_len = 0;
    unsigned char digest[EVP_MAX_MD_SIZE];
    if (HMAC(EVP_sha256(),
             secret.data(), static_cast<int>(secret.size()),
             reinterpret_cast<const unsigned char*>(signing_input.data()),
             signing_input.size(), digest, &digest_len) == nullptr) {
        err = "hmac failed";
        return false;
    }
    const std::string expected_sig = toBase64Url(
        std::string(reinterpret_cast<const char*>(digest), digest_len));
    if (expected_sig.size() != parts[2].size()
        || CRYPTO_memcmp(expected_sig.data(), parts[2].data(), expected_sig.size()) != 0) {
        err = "token signature invalid";
        return false;
    }

    out.iss = payload.get("iss", "").asString();
    if (out.iss != issuer) {
        err = "token issuer mismatch";
        return false;
    }

    const int64_t now = static_cast<int64_t>(time(nullptr));
    out.exp = payload.get("exp", 0).asInt64();
    out.iat = payload.get("iat", 0).asInt64();
    if (out.exp <= 0 || now > out.exp + clock_skew_seconds) {
        err = "token expired";
        return false;
    }
    if (out.iat > 0 && now + clock_skew_seconds < out.iat) {
        err = "token iat invalid";
        return false;
    }

    std::string sub = payload.get("sub", "").asString();
    if (sub.empty() && (payload["sub"].isUInt64() || payload["sub"].isInt64())) {
        sub = payload["sub"].asString();
    }
    if (!parseUInt64(sub, out.userId) || out.userId == 0) {
        err = "token sub invalid";
        return false;
    }

    out.deviceId = payload.get("did", "").asString();
    out.rawPayload = payload;
    return true;
}

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

} // namespace agent
