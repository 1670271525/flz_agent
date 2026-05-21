#pragma once
#ifndef __FLZ_AGENT_UTIL_UUID_H__
#define __FLZ_AGENT_UTIL_UUID_H__

/**
 * @file    uuid.h
 * @brief   生成会话与请求唯一标识
 * @date    2026-05
 * @note    RAII: 纯工具类，无资源持有
 */

#include <string>

namespace agent {

class Uuid {
public:
    static std::string generate(const std::string& prefix);
};

} // namespace agent

#endif // __FLZ_AGENT_UTIL_UUID_H__
