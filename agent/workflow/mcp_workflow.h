#pragma once
#ifndef __FLZ_AGENT_WORKFLOW_MCP_WORKFLOW_H__
#define __FLZ_AGENT_WORKFLOW_MCP_WORKFLOW_H__

/**
 * @file    mcp_workflow.h
 * @brief   MCP 工作流（占位实现）
 * @date    2026-05
 * @note    RAII: 复用基础工作流资源
 */

#include "agent/workflow/i_workflow.h"

namespace agent {

class McpWorkflow : public IWorkflow {
public:
    std::string type() const override;
    int handle(WorkflowContext& ctx) override;
};

} // namespace agent

#endif // __FLZ_AGENT_WORKFLOW_MCP_WORKFLOW_H__
