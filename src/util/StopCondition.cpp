#include "util/StopCondition.h"

std::unique_lock<std::mutex> StopCondition::wait(std::function<bool(void)> stop_waiting)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    auto stop = [&]{
        return m_stop.stop_requested() || stop_waiting();
    };

    // Handles spurious wakeups with stop.
    m_condition.wait(lock, m_stop, stop);

    return lock;
}

std::unique_lock<std::mutex> StopCondition::wait()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    auto stop = [&]{
        return m_stop.stop_requested();
    };

    // Handles spurious wakeups with stop.
    m_condition.wait(lock, m_stop, stop);

    return lock;
}

std::unique_lock<std::mutex> StopCondition::wait_until(
    Time::Timestamp timestamp,
    std::function<bool(void)> stop_waiting
) {
    std::unique_lock<std::mutex> lock(m_mutex);

    auto stop = [&]{
        return (
            Time::now() >= timestamp ||
            stop_waiting() ||
            m_stop.stop_requested()
        );
    };

    // Handles spurious wakeups with stop.
    m_condition.wait_until(lock, m_stop, timestamp, stop);

    return lock;
}

std::unique_lock<std::mutex> StopCondition::wait_until(Time::Timestamp timestamp)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    auto stop = [&]{
        return Time::now() >= timestamp || m_stop.stop_requested();
    };

    // Handles spurious wakeups with stop.
    m_condition.wait_until(lock, m_stop, timestamp, stop);

    return lock;
}
