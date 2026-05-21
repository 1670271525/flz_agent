#pragma once
#ifndef __FLZ_AGENT_CONFIG_LLM_CONFIG_H__
#define __FLZ_AGENT_CONFIG_LLM_CONFIG_H__

/**
 * @file    llm_config.h
 * @brief   LLM 提供商配置定义与读取
 * @date    2026-05
 * @note    RAII: 配置值为纯值对象，不持有网络连接
 */

#include "include/config.h"

#include <map>
#include <string>

namespace agent {

struct LlmProviderConfig {
    std::string baseUrl;
    std::string ssePath;
    std::string apiKey;
    std::string modelId;
    uint64_t timeoutMs = 60000;
    int maxRetry = 1;
    bool keepalive = true;

    bool operator==(const LlmProviderConfig& other) const {
        return baseUrl == other.baseUrl
            && ssePath == other.ssePath
            && apiKey == other.apiKey
            && modelId == other.modelId
            && timeoutMs == other.timeoutMs
            && maxRetry == other.maxRetry
            && keepalive == other.keepalive;
    }
};

struct LlmConfigData {
    std::string defaultProvider;
    std::map<std::string, LlmProviderConfig> providers;

    bool operator==(const LlmConfigData& other) const {
        return defaultProvider == other.defaultProvider
            && providers == other.providers;
    }
};

class LlmConfig {
public:
    static LlmConfigData get();
    static LlmProviderConfig getProvider(const std::string& name);
};

} // namespace agent

namespace flz {

template<>
class LexicalCast<std::string, agent::LlmProviderConfig> {
public:
    agent::LlmProviderConfig operator()(const std::string& v);
};

template<>
class LexicalCast<agent::LlmProviderConfig, std::string> {
public:
    std::string operator()(const agent::LlmProviderConfig& v);
};

template<>
class LexicalCast<std::string, agent::LlmConfigData> {
public:
    agent::LlmConfigData operator()(const std::string& v);
};

template<>
class LexicalCast<agent::LlmConfigData, std::string> {
public:
    std::string operator()(const agent::LlmConfigData& v);
};

} // namespace flz

#endif // __FLZ_AGENT_CONFIG_LLM_CONFIG_H__
