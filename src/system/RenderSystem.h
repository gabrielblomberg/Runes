#pragma once

#include <thread>
#include <vector>
#include <stop_token>
#include <memory>
#include <mutex>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Components.h"

class RenderSystem;

/**
 * @brief A scene.
 */
class Scene
{
public:

    /// Cannot copy a scene.
    inline Scene(const Scene&) = delete;
    inline Scene operator=(const Scene&) = delete;

    /**
     * @brief Move a scene.
     */
    inline Scene(Scene &&scene)
    {
        std::unique_lock lock(scene.m_mutex);
        m_entities = std::move(scene.m_entities);
    }

    /**
     * @brief Move a scene.
     */
    inline Scene &operator=(Scene &&scene)
    {
        std::unique_lock lock(scene.m_mutex);
        m_entities = std::move(scene.m_entities);
        return *this;
    }

    /**
     * @brief Remove all components from the scene.`
     */
    ~Scene();

    /**
     * @brief Add an entity to a scene.
     */
    void add_entity(Entity entity);

    /**
     * @brief Remove an entity from the scene.
     */
    void remove_entity(Entity entity);

private:

    friend class RenderSystem;

    /**
     * @brief Initialise a new scene.
     * @param render_system Pointer to the owning rendering system.
     */
    Scene(RenderSystem *render_system)
        : m_render_system(render_system)
    {}

    /// The rendering
    RenderSystem *m_render_system;

    /// The entities in the scene.
    std::vector<Entity> m_entities;

    /// Mutex for concurrent access to the scene.
    std::mutex m_mutex;
};

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

/**
 * @brief Responsible for rendering 
 */
class RenderSystem
{
public:

    /**
     * @brief Create the rendering system.
     * 
     * @param ecs The entity component system.
     * @param title The title of the rendering window.
     */
    RenderSystem(ECS &ecs, std::string &&title);

    ~RenderSystem();

    /**
     * @brief Start the renderer.
     */
    void start(std::stop_token stop);

    /**
     * @brief Get the width of the screen in pixels.
     */
    inline int screen_width() {
        return m_window->getSize().x;
    }

    /**
     * @brief Get the height of the screen in pixels.
     */
    inline int screen_height() {
        return m_window->getSize().y;
    }

    /**
     * @brief Create a new scene.
     * 
     * @returns 
     */
    inline Scene scene_create()
    {
        return Scene(this);
    }

    /**
     * @brief Set the scene.
     * @param scene The scene to render.
     */
    void scene_set(Scene &scene);

    /**
     * @brief Lock the rendering system.
     * @returns A lock on the rendering system.
     */
    inline RenderLock lock() {
        return RenderLock(m_window.get(), &m_mutex);
    }

private:

    friend class Scene;
    friend class EventSystem;

    /**
     * @brief Thread responsible for rendering and changing scene.
     * @param stop The signal to stop the rendering thread.
     */
    void render_thread(std::stop_token stop);

    /// The window containing graphics.
    std::unique_ptr<sf::RenderWindow> m_window;

    /// Pointer to the entity component system.
    ECS *m_ecs;

    /// The current scene.
    Scene *m_current_scene;

    /// The view of the thread.
    std::jthread m_render_thread;

    /// Mutex protecting concurrent access to the window.
    std::mutex m_mutex;
};
