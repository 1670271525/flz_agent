#include "agent/util/json_helper.h"

#include "include/json_util.h"

namespace agent {

bool JsonHelper::parse(const std::string& text, Json::Value& out) {
    return flz::JsonUtil::FromString(out, text);
}

std::string JsonHelper::toString(const Json::Value& v) {
    return flz::JsonUtil::ToString(v);
}

Json::Value JsonHelper::makeError(int code, const std::string& msg) {
    Json::Value root;
    root["code"] = code;
    root["msg"] = msg;
    return root;
}

} // namespace agent
