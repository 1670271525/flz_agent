#pragma once
#ifndef __FLZ_AGENT_UTIL_STRING_UTIL_H__
#define __FLZ_AGENT_UTIL_STRING_UTIL_H__

/**
 * @file    string_util.h
 * @brief   字符串工具: trim/split/脱敏
 * @date    2026-05
 * @note    RAII: 纯工具类，无动态资源持有
 */

#include <string>
#include <vector>

namespace agent {

/**
 * @class StringUtil
 * @brief 业务侧字符串处理辅助
 */
class StringUtil {
public:
    static std::string trim(const std::string& input);
    static std::vector<std::string> split(const std::string& input, char delimiter);
    static std::string redact(const std::string& input);
};

} // namespace agent

#endif // __FLZ_AGENT_UTIL_STRING_UTIL_H__
