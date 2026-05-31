#include "agent/llm/sse_event_parser.h"

namespace agent {

std::vector<std::string> SseEventParser::append(const std::string& chunk) {
    std::string normalized = chunk;
    for (size_t i = 0; i + 1 < normalized.size(); ++i) {
        if (normalized[i] == '\r' && normalized[i + 1] == '\n') {
            normalized.erase(i, 1);
        }
    }
    m_buffer.append(normalized);
    std::vector<std::string> events;
    const std::string sep = "\n\n";
    size_t pos = std::string::npos;
    while ((pos = m_buffer.find(sep)) != std::string::npos) {
        std::string event = m_buffer.substr(0, pos);
        if (!event.empty()) {
            events.push_back(event);
        }
        m_buffer.erase(0, pos + sep.size());
    }
    return events;
}

} // namespace agent
