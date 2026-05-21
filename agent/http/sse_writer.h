#pragma once
#ifndef __FLZ_AGENT_HTTP_SSE_WRITER_H__
#define __FLZ_AGENT_HTTP_SSE_WRITER_H__

/**
 * @file    sse_writer.h
 * @brief   SSE 输出 RAII 封装
 * @date    2026-05
 * @note    RAII: 构造写响应头，析构自动 close
 */

#include "http/http_session.h"

#include <memory>
#include <string>

namespace agent {

class SseWriter {
public:
    explicit SseWriter(flz::http::HttpSession::ptr session);
    ~SseWriter();

    bool sendEvent(const std::string& event, const std::string& json_data);
    bool sendComment(const std::string& s);
    bool isClientAlive() const;
    void close();

private:
    bool writeRaw(const std::string& s);

private:
    flz::http::HttpSession::ptr m_session;
    bool m_headerSent = false;
    bool m_closed = false;
};

} // namespace agent

#endif // __FLZ_AGENT_HTTP_SSE_WRITER_H__
