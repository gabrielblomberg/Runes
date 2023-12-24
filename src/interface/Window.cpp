#include "interface/Window.h"

Window::Window(std::string &&title)
    : m_window()
{
    // Determine the largest fullscreen mode.
    auto modes = sf::VideoMode::getFullscreenModes();
    auto mode = std::max_element(
        modes.begin(),
        modes.end(),
        [](sf::VideoMode &a, sf::VideoMode &b) {
            return (a.width * a.height) < (b.width * b.height);
        }
    );

    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(mode->width, mode->height),
        title.c_str(),
        sf::Style::Default
    );

    m_window->setActive(false);
}

Window::~Window()
{
    WindowLock window_lock(m_window.get(), &m_mutex);
    m_window->close();
}
