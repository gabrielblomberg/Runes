#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <stop_token>

#include "util/Time.h"

/**
 * @brief Wrapper around a std::stop_token with methods for waiting on the
 * stop token or predicates.
 */
class StopCondition
{
public:

    /**
     * @brief Create a new stop condition from a stop token.
     * @param stop The stop token to stop waiting on when requested.
     */
    StopCondition(std::stop_token stop)
        : m_stop(stop)
    {}

    /**
     * @brief Wait for stop signal.
     */
    std::unique_lock<std::mutex> wait();

    /**
     * @brief Wait for the stop signal or predicate.
     * 
     * @param stop_waiting A function that returns true when waiting should be
     * stopped.
     * 
     * @return A lock on the stop condition mutex. 
     */
    std::unique_lock<std::mutex> wait(
        std::function<bool(void)> stop_waiting
    );

    /**
     * @brief Wait until a timestamp or the stop signal.
     * 
     * @param timestamp The timestamp at which to stop waiting.
     * 
     * @return A lock on the stop condition mutex. 
     */
    std::unique_lock<std::mutex> wait_until(
        Time::Timestamp timestamp
    );

    /**
     * @brief Wait until a timestamp, the stop signal or predicate.
     * 
     * @param timestamp The timestamp at which to stop waiting.
     * @param stop_waiting A function that returns true to stop waiting.
     * 
     * @return A lock on the stop condition mutex. 
     */
    std::unique_lock<std::mutex> wait_until(
        Time::Timestamp timestamp,
        std::function<bool(void)> stop_waiting
    );

    /**
     * @brief Wait for a duration or the stop signal.
     * 
     * @param duration The duration to wait for.
     * 
     * @return A lock on the stop condition mutex.
     */
    inline std::unique_lock<std::mutex> wait_for(Time::Duration duration) {
        return wait_until(Time::now() + duration);
    }

    /**
     * @brief Wait for a duration, the stop signal or a predicate.
     * 
     * @param duration The duration to wait for.
     * @param stop_waiting A function that returns true to stop waiting.
     * 
     * @return A lock on the stop condition mutex.
     */
    inline std::unique_lock<std::mutex> wait_for(
        Time::Duration duration,
        std::function<bool(void)> stop_waiting
    ) {
        return wait_until(Time::now() + duration, stop_waiting);
    }

private:

    /// Signal to stop waiting permanently.
    std::stop_token m_stop;

    /// Condition variable to wait on for stop signal
    std::condition_variable_any m_condition;

    /// Mutex to lock on condition variable.
    std::mutex m_mutex;
};
