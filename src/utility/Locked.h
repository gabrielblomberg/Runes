#pragma once

#include <mutex>

template<typename T>
class Locked
{
public:

    inline Locked(T &object, std::mutex &mutex)
        : m_object(&object)
        , m_lock(m_mutex)
    {}

    inline T &operator*() {
        return *m_object;
    }

    inline T &operator->() {
        return *m_object;
    }

private:

    // Object being locked.
    T *m_object;

    // The lock.
    std::unique_lock<std::mutex> m_lock;
};
