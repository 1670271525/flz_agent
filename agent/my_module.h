#pragma once
#ifndef __FLZ_AGENT_MY_MODULE_H__
#define __FLZ_AGENT_MY_MODULE_H__

/**
 * @file    my_module.h
 * @brief   flz_agent 动态模块入口，负责注册 Servlet 到 Dispatch
 * @date    2026-05
 * @note    RAII: 由 flz::Module 生命周期托管
 */

#include "include/module.h"

namespace agent {

/**
 * @class MyModule
 * @brief 项目主模块：onServerReady 时向 HttpServer 的 ServletDispatch 注册路由
 */
class MyModule : public flz::Module {
public:
    typedef std::shared_ptr<MyModule> ptr;

    MyModule();
    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
};

} // namespace agent

#endif // __FLZ_AGENT_MY_MODULE_H__
