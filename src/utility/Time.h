#pragma once

#include <chrono>

using namespace std::chrono_literals;

namespace Time {

using Clock = std::chrono::high_resolution_clock;

using Timestamp = std::chrono::high_resolution_clock::time_point;

using Duration = std::chrono::high_resolution_clock::duration;

/**
 * @brief Get the current time epoch timestamp.
 * @return The current time.
 */
inline Timestamp now() {
    return std::chrono::high_resolution_clock::now();
}

} // namespace Time
