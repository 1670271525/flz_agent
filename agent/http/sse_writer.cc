#include "agent/http/sse_writer.h"

#include "include/log.h"

namespace agent {

static flz::Logger::ptr g_logger = FLZ_LOG_ROOT();

SseWriter::SseWriter(flz::http::HttpSession::ptr session)
    : m_session(session) {
    const std::string header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/event-stream\r\n"
        "Cache-Control: no-cache\r\n"
        "Connection: close\r\n\r\n";
    m_headerSent = writeRaw(header);
}

SseWriter::~SseWriter() {
    close();
}

bool SseWriter::sendEvent(const std::string& event, const std::string& json_data) {
    if (m_closed || !m_headerSent) {
        return false;
    }
    std::string buf = "event: " + event + "\n";
    buf += "data: " + json_data + "\n\n";
    return writeRaw(buf);
}

bool SseWriter::sendComment(const std::string& s) {
    if (m_closed || !m_headerSent) {
        return false;
    }
    return writeRaw(": " + s + "\n\n");
}

bool SseWriter::isClientAlive() const {
    return m_session && m_session->isConnected() && !m_closed;
}

void SseWriter::close() {
    if (m_closed) {
        return;
    }
    m_closed = true;
    if (m_session) {
        m_session->close();
    }
}

bool SseWriter::writeRaw(const std::string& s) {
    if (!m_session) {
        return false;
    }
    int rt = m_session->writeFixSize(s.c_str(), s.size());
    if (rt <= 0) {
        FLZ_LOG_WARN(g_logger) << "sse write failed, rt=" << rt;
        m_closed = true;
        return false;
    }
    return true;
}

} // namespace agent
