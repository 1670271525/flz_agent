#pragma once
#ifndef __FLZ_AGENT_WORKFLOW_I_WORKFLOW_H__
#define __FLZ_AGENT_WORKFLOW_I_WORKFLOW_H__

/**
 * @file    i_workflow.h
 * @brief   工作流接口定义
 * @date    2026-05
 * @note    RAII: 接口层不持有资源，生命周期由 shared_ptr 管理
 */

#include <memory>
#include <string>

namespace agent {

class WorkflowContext;

class IWorkflow {
public:
    typedef std::shared_ptr<IWorkflow> ptr;
    virtual ~IWorkflow() {}
    virtual std::string type() const = 0;
    virtual int handle(WorkflowContext& ctx) = 0;
};

} // namespace agent

#endif // __FLZ_AGENT_WORKFLOW_I_WORKFLOW_H__
