#pragma once

#include <thread>
#include <vector>
#include <stop_token>
#include <memory>
#include <mutex>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "system/EntitySystem.h"
#include "system/render/RenderLock.h"
#include "system/render/Scene.h"

/**
 * @brief Responsible for rendering 
 */
class RenderSystem
{
public:

    /**
     * @brief Create the rendering system.
     * @param title The title of the rendering window.
     */
    RenderSystem(EntitySystem *entity_system, std::stop_token stop);

    ~RenderSystem();

    /**
     * @brief Start the renderer.
     */
    void start();

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

    /**
     * @brief Thread responsible for rendering and changing scene.
     * @param stop The signal to stop the rendering thread.
     */
    void main();

    std::stop_token m_stop;

    /// The window containing graphics.
    std::unique_ptr<sf::RenderWindow> m_window;

    /// The entity component system.
    EntitySystem *m_entity_system;

    /// The current scene.
    Scene *m_current_scene;

    /// The view of the thread.
    std::jthread m_thread;

    /// Mutex protecting concurrent access to the window.
    std::mutex m_mutex;
};
