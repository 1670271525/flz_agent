#include "agent/workflow/workflow_dispatcher.h"

namespace agent {

WorkflowDispatcher* WorkflowDispatcher::GetInstance() {
    return WorkflowDispatcherSingleton::GetInstance();
}

void WorkflowDispatcher::registerWorkflow(IWorkflow::ptr workflow) {
    if (!workflow) {
        return;
    }
    RWMutexType::WriteLock lock(m_mutex);
    m_workflows[workflow->type()] = workflow;
}

IWorkflow::ptr WorkflowDispatcher::find(const std::string& type) {
    RWMutexType::ReadLock lock(m_mutex);
    std::unordered_map<std::string, IWorkflow::ptr>::const_iterator it = m_workflows.find(type);
    if (it != m_workflows.end()) {
        return it->second;
    }
    it = m_workflows.find("chat");
    if (it != m_workflows.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace agent
