#pragma once
#ifndef __FLZ_AGENT_UTIL_JSON_HELPER_H__
#define __FLZ_AGENT_UTIL_JSON_HELPER_H__

/**
 * @file    json_helper.h
 * @brief   Json 便捷封装
 * @date    2026-05
 * @note    RAII: 纯工具封装，底层依赖 flz::JsonUtil
 */

#include <json/json.h>
#include <string>

namespace agent {

class JsonHelper {
public:
    static bool parse(const std::string& text, Json::Value& out);
    static std::string toString(const Json::Value& v);
    static Json::Value makeError(int code, const std::string& msg);
};

} // namespace agent

#endif // __FLZ_AGENT_UTIL_JSON_HELPER_H__
