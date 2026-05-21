#pragma once
#ifndef __FLZ_AGENT_WORKFLOW_WORKFLOW_DISPATCHER_H__
#define __FLZ_AGENT_WORKFLOW_WORKFLOW_DISPATCHER_H__

/**
 * @file    workflow_dispatcher.h
 * @brief   工作流注册与路由分发
 * @date    2026-05
 * @note    RAII: 单例内使用 RWMutex 保护共享映射
 */

#include "agent/workflow/i_workflow.h"
#include "include/mutex.h"
#include "include/singleton.h"

#include <string>
#include <unordered_map>

namespace agent {

class WorkflowDispatcher {
public:
    typedef flz::RWMutex RWMutexType;
    static WorkflowDispatcher* GetInstance();

    void registerWorkflow(IWorkflow::ptr workflow);
    IWorkflow::ptr find(const std::string& type);

private:
    RWMutexType m_mutex;
    std::unordered_map<std::string, IWorkflow::ptr> m_workflows;
};

typedef flz::Singleton<WorkflowDispatcher> WorkflowDispatcherSingleton;

} // namespace agent

#endif // __FLZ_AGENT_WORKFLOW_WORKFLOW_DISPATCHER_H__
