#include "agent/util/time_util.h"

#include <chrono>

namespace agent {

int64_t TimeUtil::nowMs() {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

int64_t TimeUtil::nowSec() {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(now).count();
}

} // namespace agent
