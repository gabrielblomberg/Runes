#include "Application.h"

Application::Application()
    : m_stop_source()
    , m_ecs()
    , m_window(create_window())
    , m_event_manager(m_window.get(), m_stop_source.get_token())
    , m_rune_system(&m_ecs, &m_event_manager, m_stop_source.get_token())
    , m_render_system(&m_ecs, &m_event_manager, m_stop_source.get_token())
    , m_interface_system(&m_ecs, &m_event_manager, m_stop_source.get_token())
{}

Application::~Application()
{
    m_window->close();
}

void Application::create_window()
{
    // Determine the largest fullscreen mode.
    auto modes = sf::VideoMode::getFullscreenModes();
    auto mode = std::max_element(
        modes.begin(),
        modes.end(),
        [](sf::VideoMode &a, sf::VideoMode &b) {
            return (a.size.x * a.size.y) < (b.size.x * b.size.y);
        }
    );

    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(mode->size),
        "Runes",
        sf::Style::Default
    );

    m_window->setActive(false);
}

void Application::main()
{
    auto stop = m_stop_source.get_token();

    for (;;) {
        m_event_system.step();
        if (stop.stop_requested())
            break;
        m_rune_system.step();
        m_interface_system.step();
        m_render_system.step(0.0);
    }
}
