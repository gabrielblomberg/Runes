#include "Application.h"

#include <SFML/Window/Event.hpp>

Application::Application()
    : m_stop_source()
    , m_ecs()
    , m_ecs_events()
    , m_window(create_window())
    , m_interface_system(&m_ecs, &m_ecs_events, m_stop_source.get_token())
{
    create_window();

    m_stop_subscription = m_event_manager.key.subscribe_async([&](const Event::Key& key) {
        if (key.key == sf::Keyboard::Key::Escape) {
            m_stop_source.request_stop();
        }
    });
}

Application::~Application() { m_window->close(); }

std::unique_ptr<sf::RenderWindow> Application::create_window()
{
    // Determine the largest fullscreen mode.
    auto modes = sf::VideoMode::getFullscreenModes();
    auto mode =
        std::max_element(modes.begin(), modes.end(), [](sf::VideoMode& a, sf::VideoMode& b) {
            return (a.size.x * a.size.y) < (b.size.x * b.size.y);
        });

    auto window =
        std::make_unique<sf::RenderWindow>(sf::VideoMode(mode->size), "Runes", sf::Style::Default);

    window->setActive(false);
    return window;
}

void Application::main()
{
    auto stop = m_stop_source.get_token();

    for (;;) {
        m_event_manager.step();
        if (stop.stop_requested())
            break;

        m_interface_system.step();
        m_rune_system.step();
        m_render_system.step(0.0);
    }
}

void Application::step_input()
{
    for (;;) {
        auto event = m_window->pollEvent();
        if (!event || m_stop.stop_requested())
            return;

        if (auto key = event->getIf<sf::Event::KeyPressed>()) {
            m_ecs.push_event<Event::Key>(key->code, true);
        }
        else if (auto key = event->getIf<sf::Event::KeyReleased>()) {
            m_ecs.push_event<Event::Key>(key->code, false);
        }
        else if (auto mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            m_ecs.push_event<Event::Click>(Event::Click{
                .x = mouse->position.x,
                .y = mouse->position.y,
                .pressed = true,
                .button = mouse->button
            });
        }
        else if (auto mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
            m_ecs.push_event<Event::Click>(Event::Click{
                .x = mouse->position.x,
                .y = mouse->position.y,
                .pressed = false,
                .button = mouse->button
            });
        }
        else if (auto mouse = event->getIf<sf::Event::MouseMoved>()) {
            m_ecs.push_event<Event::Mouse>(mouse->position.x, mouse->position.y);
        }
    }
}
