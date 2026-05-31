#include "agent/util/json_helper.h"

#include "include/json_util.h"

namespace agent {

bool JsonHelper::parse(const std::string& text, Json::Value& out) {
    return flz::JsonUtil::FromString(out, text);
}

std::string JsonHelper::toString(const Json::Value& v) {
    Json::StreamWriterBuilder writer_builder;
    writer_builder["indentation"] = "";
    writer_builder["commentStyle"] = "None";
    writer_builder["emitUTF8"] = true;
    std::string out = Json::writeString(writer_builder, v);
    if (!out.empty() && out[out.size() - 1] == '\n') {
        out.erase(out.size() - 1);
    }
    return out;
}

Json::Value JsonHelper::makeError(int code, const std::string& msg) {
    Json::Value root;
    root["code"] = code;
    root["msg"] = msg;
    return root;
}

} // namespace agent
