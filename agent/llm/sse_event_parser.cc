#include "agent/llm/sse_event_parser.h"

namespace agent {

std::vector<std::string> SseEventParser::append(const std::string& chunk) {
    m_buffer.append(chunk);
    std::vector<std::string> events;
    const std::string sep = "\n\n";
    size_t pos = std::string::npos;
    while ((pos = m_buffer.find(sep)) != std::string::npos) {
        events.push_back(m_buffer.substr(0, pos));
        m_buffer.erase(0, pos + sep.size());
    }
    return events;
}

} // namespace agent
