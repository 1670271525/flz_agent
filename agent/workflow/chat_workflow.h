#pragma once
#ifndef __FLZ_AGENT_WORKFLOW_CHAT_WORKFLOW_H__
#define __FLZ_AGENT_WORKFLOW_CHAT_WORKFLOW_H__

/**
 * @file    chat_workflow.h
 * @brief   默认聊天工作流
 * @date    2026-05
 * @note    RAII: 处理过程依赖上下文对象自动回收
 */

#include "agent/workflow/i_workflow.h"

namespace agent {

class ChatWorkflow : public IWorkflow {
public:
    std::string type() const override;
    int handle(WorkflowContext& ctx) override;
};

} // namespace agent

#endif // __FLZ_AGENT_WORKFLOW_CHAT_WORKFLOW_H__
