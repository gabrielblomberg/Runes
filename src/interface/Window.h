#pragma once

#include <memory>
#include <mutex>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

/**
 * @brief Only one thread can draw to the window at once. This class provides a
 * RAII style lock on the window during access.
 */
class WindowLock
{
public:

    /**
     * @brief Constructs a window lock by move.
     * @param other The window lock to move to this one.
     */
    inline WindowLock(WindowLock &&other)
        : m_window(other.m_window)
        , m_mutex(std::move(other.m_mutex))
    {}

    /**
     * @brief Disable the window as the active OpenGL target and unlock.
     */
    inline ~WindowLock()
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
     * @brief Get a pointer to the locked window.
     */
    inline sf::RenderWindow *get() {
        return m_window;
    }

private:

    friend class Window;

    /**
     * @brief Lock a window and set this thread to the active OpenGL target.
     * 
     * @param window The window to lock.
     * @param mutex The mutex to use to protect accessing the window.
     */
    inline WindowLock(sf::RenderWindow *window, std::mutex *mutex)
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

/**
 * @brief Wrapper around sf::RenderWindow with some basic utilities.
 */
class Window
{
public:

    /**
     * @brief Create a new window.
     * 
     * @param title The title of the window.
     */
    Window(std::string &&title);

    ~Window();

    /**
     * @brief Get a lock on the window.
     */
    inline WindowLock lock() {
        return WindowLock(m_window.get(), &m_mutex);
    }

    /**
     * @brief Returns a dangerous pointer to the window. Do not use this to draw
     * to the window, it will not work.
     * 
     * @return A pointer to the window instance.
     */
    inline sf::RenderWindow *operator->() {
        return m_window.get();
    }

private:

    /// The window containing graphics.
    std::unique_ptr<sf::RenderWindow> m_window;

    /// Mutex protecting concurrent access to the window.
    std::mutex m_mutex;
};
