#pragma once
#ifndef __FLZ_AGENT_WORKFLOW_CODE_WORKFLOW_H__
#define __FLZ_AGENT_WORKFLOW_CODE_WORKFLOW_H__

/**
 * @file    code_workflow.h
 * @brief   代码生成工作流（当前复用聊天链路）
 * @date    2026-05
 * @note    RAII: 复用 ChatWorkflow 的资源管理语义
 */

#include "agent/workflow/i_workflow.h"

namespace agent {

class CodeWorkflow : public IWorkflow {
public:
    std::string type() const override;
    int handle(WorkflowContext& ctx) override;
};

} // namespace agent

#endif // __FLZ_AGENT_WORKFLOW_CODE_WORKFLOW_H__
