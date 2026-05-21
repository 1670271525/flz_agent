#pragma once
#ifndef __FLZ_AGENT_MODULE_AGENT_MODULE_H__
#define __FLZ_AGENT_MODULE_AGENT_MODULE_H__

/**
 * @file    agent_module.h
 * @brief   Agent 模块入口: 负责注册配置、工作流与 HTTP 路由
 * @date    2026-05
 * @note    RAII: 由 flz::Module 生命周期托管，资源通过智能指针自动释放
 */

#include "include/module.h"

namespace agent {

/**
 * @class AgentModule
 * @brief flz_agent 动态模块入口
 * @note  线程安全性: 生命周期由框架单线程驱动，内部注册依赖线程安全单例
 */
class AgentModule : public flz::Module {
public:
    typedef std::shared_ptr<AgentModule> ptr;

    AgentModule();
    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
};

} // namespace agent

#endif // __FLZ_AGENT_MODULE_AGENT_MODULE_H__
