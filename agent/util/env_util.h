#pragma once
#ifndef __FLZ_AGENT_UTIL_ENV_UTIL_H__
#define __FLZ_AGENT_UTIL_ENV_UTIL_H__

/**
 * @file    env_util.h
 * @brief   配置占位符 ${ENV_VAR} 展开
 * @date    2026-05
 */

#include <string>

namespace agent {

class EnvUtil {
public:
    /// 是否为 ${VAR} 形式占位符
    static bool isPlaceholder(const std::string& raw);

    /// 提取占位符变量名；非占位符返回空串
    static std::string placeholderName(const std::string& raw);

    /// 展开 ${VAR}；非占位符原样返回；占位符且环境变量缺失时返回空串
    static std::string expand(const std::string& raw);
};

} // namespace agent

#endif // __FLZ_AGENT_UTIL_ENV_UTIL_H__
