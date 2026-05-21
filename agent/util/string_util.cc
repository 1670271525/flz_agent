#include "agent/util/string_util.h"

#include "include/util.h"

namespace agent {

std::string StringUtil::trim(const std::string& input) {
    return flz::StringUtil::Trim(input);
}

std::vector<std::string> StringUtil::split(const std::string& input, char delimiter) {
    return flz::split(input, delimiter);
}

std::string StringUtil::redact(const std::string& input) {
    if (input.size() <= 8) {
        return "***";
    }
    return input.substr(0, 3) + "***" + input.substr(input.size() - 3);
}

} // namespace agent
