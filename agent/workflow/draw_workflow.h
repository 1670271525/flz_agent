#pragma once
#ifndef __FLZ_AGENT_WORKFLOW_DRAW_WORKFLOW_H__
#define __FLZ_AGENT_WORKFLOW_DRAW_WORKFLOW_H__

/**
 * @file    draw_workflow.h
 * @brief   绘图工作流（占位实现）
 * @date    2026-05
 * @note    RAII: 复用基础工作流资源
 */

#include "agent/workflow/i_workflow.h"

namespace agent {

class DrawWorkflow : public IWorkflow {
public:
    std::string type() const override;
    int handle(WorkflowContext& ctx) override;
};

} // namespace agent

#endif // __FLZ_AGENT_WORKFLOW_DRAW_WORKFLOW_H__
