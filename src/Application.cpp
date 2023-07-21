#include "Application.h"

#include "states/GameState.h"

Application::Application()
    : m_stop()
    , m_window("Runes")
    , m_messenger(m_stop)
{
    m_state = std::make_unique<GameState>(this, m_stop.get_token());

    m_state_thread = std::jthread(
        &Application::state_machine,
        this,
        m_stop.get_token()
    );
}

void Application::state_machine(std::stop_token stop)
{
    while (m_state && !m_stop.stop_requested()) {
        m_state = m_state->main();
    }
}

void Application::main()
{
    sf::Event event;
    while (!m_stop.stop_requested())
    {
        m_window->waitEvent(event);

        switch(event.type)
        {
            case sf::Event::Closed : {
                m_stop.request_stop();
                break;
            }
            case sf::Event::KeyPressed : {
                if (event.key.code == sf::Keyboard::Key::Escape) {
                    m_stop.request_stop();
                }
                break;
            }
            case sf::Event::MouseButtonPressed : {
                m_messenger.publish<CLICK>(
                    event.mouseButton.x,
                    event.mouseButton.y
                );
                break;
            }
            default: break;
        }
    }
}
