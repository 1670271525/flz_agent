#pragma once
#ifndef __FLZ_AGENT_CONFIG_CONFIG_LOADER_H__
#define __FLZ_AGENT_CONFIG_CONFIG_LOADER_H__

/**
 * @file    config_loader.h
 * @brief   配置加载触发器，统一注册所有 ConfigVar
 * @date    2026-05
 * @note    RAII: 静态初始化由进程生命周期托管
 */

namespace agent {

/**
 * @class ConfigLoader
 * @brief 触发配置模块静态注册
 */
class ConfigLoader {
public:
    static void init();
};

} // namespace agent

#endif // __FLZ_AGENT_CONFIG_CONFIG_LOADER_H__
