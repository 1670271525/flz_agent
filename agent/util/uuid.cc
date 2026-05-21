#include "agent/util/uuid.h"

#include "agent/util/time_util.h"

#include <atomic>
#include <sstream>

namespace agent {

std::string Uuid::generate(const std::string& prefix) {
    static std::atomic<uint64_t> s_seq(0);
    std::stringstream ss;
    ss << prefix << "_" << TimeUtil::nowMs() << "_" << ++s_seq;
    return ss.str();
}

} // namespace agent
