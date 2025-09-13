#pragma once

#include <mutex>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

/**
 * @brief Only one thread can draw to the window at once. This class provides a
 * RAII style lock on the window during access.
 */
class RenderLock
{
public:

    /**
     * @brief Constructs a window lock by move.
     * @param other The window lock to move to this one.
     */
    inline RenderLock(RenderLock &&other)
        : m_window(other.m_window)
        , m_mutex(std::move(other.m_mutex))
    {}

    /**
     * @brief Disable the window as the active OpenGL target and unlock.
     */
    inline ~RenderLock()
    {
        m_window->setActive(false);
        m_mutex->unlock();
    }

    /**
     * @brief Get a pointer to the locked window.
     */
    inline sf::RenderWindow *operator->() {
        return m_window;
    }

    /**
     * @brief Get a reference to the locked window.
     */
    inline sf::RenderWindow &operator*() {
        return *m_window;
    }

    /**
     * @brief Get a pointer to the locked window.
     */
    inline sf::RenderWindow *get() {
        return m_window;
    }

private:

    friend class RenderSystem;

    /**
     * @brief Lock a window and set this thread to the active OpenGL target.
     * 
     * @param window The window to lock.
     * @param mutex The mutex to use to protect accessing the window.
     */
    inline RenderLock(sf::RenderWindow *window, std::mutex *mutex)
        : m_window(window)
        , m_mutex(mutex)
    {
        m_mutex->lock();
        m_window->setActive(true);
    }

    /// Pointer to the render window to protect.
    sf::RenderWindow *m_window;

    /// Mutex protecting concurrent access to m_window.
    std::mutex *m_mutex;
};
