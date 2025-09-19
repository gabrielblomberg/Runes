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
#include "system/System.h"

/**
 * @brief Responsible for rendering 
 */
class RenderSystem : public System
{
public:

    /**
     * @brief Create the rendering system.
     * @param title The title of the rendering window.
     */
    RenderSystem(ECS *ecs, Messaging *messenger, std::stop_token stop);

    /**
     * @brief Closes the window.
     */
    ~RenderSystem();

    /**
     * @brief Step the renderer.
     * @param t Frame interpolation parameter.
     */
    void step(double t);

    /**
     * @brief Get the width of the screen in pixels.
     * @todo Make messageable.
     */
    inline int screen_width() {
        return m_window->getSize().x;
    }

    /**
     * @brief Get the height of the screen in pixels.
     * @todo Make messageable.
     */
    inline int screen_height() {
        return m_window->getSize().y;
    }

private:

    /**
     * @brief Thread responsible for rendering and changing scene.
     * @param stop The signal to stop the rendering thread.
     */
    void main();

    /// The window containing graphics.
    std::unique_ptr<sf::RenderWindow> m_window;

    std::mutex m_mutex;
};
