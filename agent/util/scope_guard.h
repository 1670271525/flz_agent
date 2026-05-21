#pragma once
#ifndef __FLZ_AGENT_UTIL_SCOPE_GUARD_H__
#define __FLZ_AGENT_UTIL_SCOPE_GUARD_H__

/**
 * @file    scope_guard.h
 * @brief   通用作用域守卫: 退出作用域自动执行清理回调
 * @date    2026-05
 * @note    RAII: 构造时接管回调，析构时自动触发
 */

#include <functional>
#include <utility>

namespace agent {

/**
 * @class ScopeGuard
 * @brief 作用域清理对象，用于异常路径和早返回路径统一收口
 */
class ScopeGuard {
public:
    explicit ScopeGuard(std::function<void()> cb)
        : m_cb(std::move(cb)) {
    }

    ~ScopeGuard() {
        if (m_cb && !m_dismissed) {
            m_cb();
        }
    }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

    /// 取消析构回调
    void dismiss() {
        m_dismissed = true;
    }

private:
    std::function<void()> m_cb;
    bool m_dismissed = false;
};

} // namespace agent

#endif // __FLZ_AGENT_UTIL_SCOPE_GUARD_H__
