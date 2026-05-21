#pragma once
#ifndef __FLZ_AGENT_CONFIG_AGENT_CONFIG_H__
#define __FLZ_AGENT_CONFIG_AGENT_CONFIG_H__

/**
 * @file    agent_config.h
 * @brief   Agent 业务配置定义
 * @date    2026-05
 * @note    RAII: 配置为不可共享资源的值语义对象
 */

#include "include/config.h"

#include <string>
#include <vector>

namespace agent {

struct AgentContextConfig {
    int maxRounds = 10;
    int maxTotalTokens = 8000;
    int maxMessageChars = 8000;

    bool operator==(const AgentContextConfig& other) const {
        return maxRounds == other.maxRounds
            && maxTotalTokens == other.maxTotalTokens
            && maxMessageChars == other.maxMessageChars;
    }
};

struct AgentInvocationConfig {
    int maxToolCallsPerRequest = 8;
    int maxLlmCallsPerRequest = 6;
    int requestTimeoutMs = 120000;

    bool operator==(const AgentInvocationConfig& other) const {
        return maxToolCallsPerRequest == other.maxToolCallsPerRequest
            && maxLlmCallsPerRequest == other.maxLlmCallsPerRequest
            && requestTimeoutMs == other.requestTimeoutMs;
    }
};

struct AgentWorkflowConfig {
    std::string defaultType = "chat";
    std::vector<std::string> enabled;

    bool operator==(const AgentWorkflowConfig& other) const {
        return defaultType == other.defaultType && enabled == other.enabled;
    }
};

struct AgentConfigData {
    AgentContextConfig context;
    AgentInvocationConfig invocation;
    AgentWorkflowConfig workflows;

    bool operator==(const AgentConfigData& other) const {
        return context == other.context
            && invocation == other.invocation
            && workflows == other.workflows;
    }
};

class AgentConfig {
public:
    static AgentConfigData get();
};

} // namespace agent

namespace flz {

template<>
class LexicalCast<std::string, agent::AgentConfigData> {
public:
    agent::AgentConfigData operator()(const std::string& v);
};

template<>
class LexicalCast<agent::AgentConfigData, std::string> {
public:
    std::string operator()(const agent::AgentConfigData& v);
};

} // namespace flz

#endif // __FLZ_AGENT_CONFIG_AGENT_CONFIG_H__
