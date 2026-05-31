#include "agent/util/env_util.h"

#include <cstdlib>

namespace agent {

bool EnvUtil::isPlaceholder(const std::string& raw) {
    return raw.size() >= 4 && raw[0] == '$' && raw[1] == '{' && raw[raw.size() - 1] == '}';
}

std::string EnvUtil::placeholderName(const std::string& raw) {
    if (!isPlaceholder(raw)) {
        return "";
    }
    return raw.substr(2, raw.size() - 3);
}

std::string EnvUtil::expand(const std::string& raw) {
    if (!isPlaceholder(raw)) {
        return raw;
    }
    const std::string key = placeholderName(raw);
    const char* env = std::getenv(key.c_str());
    if (env && env[0] != '\0') {
        return std::string(env);
    }
    return "";
}

} // namespace agent
