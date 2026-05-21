#pragma once
#ifndef __FLZ_AGENT_UTIL_TIME_UTIL_H__
#define __FLZ_AGENT_UTIL_TIME_UTIL_H__

/**
 * @file    time_util.h
 * @brief   时间工具: 秒/毫秒时间戳
 * @date    2026-05
 * @note    RAII: 纯工具类，无资源持有
 */

#include <stdint.h>

namespace agent {

class TimeUtil {
public:
    static int64_t nowMs();
    static int64_t nowSec();
};

} // namespace agent

#endif // __FLZ_AGENT_UTIL_TIME_UTIL_H__
