#include "Application.h"

#include "core/GameState.h"
#include "util/StopCondition.h"

Application::Application()
    : m_stop()
    , m_window("Runes")
    , m_messenger(m_stop)
{
    m_state_thread = std::jthread(&Application::run, this, m_stop.get_token());
}

void Application::run(std::stop_token stop)
{
    std::unique_ptr<State> state = std::make_unique<GameState>(this);

    while (state) {
        auto extend = state->run(StopCondition(m_stop.get_token()));

        for (auto &state : extend)
            m_states.push_back(std::move(state));

        if (!m_states.empty()) {
            state = std::move(m_states.back());
            m_states.pop_back();
            continue;
        }

        state = nullptr;
        break;
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
            case sf::Event::Closed: {
                m_stop.request_stop();
                break;
            }
            case sf::Event::KeyReleased:
            case sf::Event::KeyPressed: {
                if (event.key.code == sf::Keyboard::Key::Escape) {
                    m_stop.request_stop();
                }
                else {
                    m_messenger.publish<KEY>(
                        event.key.code,
                        event.type == sf::Event::KeyPressed
                    );
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
            case sf::Event::MouseButtonPressed: {
                m_messenger.publish<CLICK>(
                    event.mouseButton.x,
                    event.mouseButton.y,
                    event.type == sf::Event::MouseButtonPressed,
                    event.mouseButton.button
                );
                break;
            }
            case sf::Event::MouseMoved: {
                m_messenger.publish<MOUSE>(
                    event.mouseMove.x,
                    event.mouseMove.y
                );
                break;
            }
            default: break;
        }
    }
}
