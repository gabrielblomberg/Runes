#include "system/EventSystem.h"

EventSystem::EventSystem(RenderSystem &render_system)
    : m_window(render_system.lock().get())
    , m_stop()
    , m_messenger()
{}

void EventSystem::run()
{
    while (!m_stop.stop_requested())
    {
        auto event = m_window->waitEvent();
        if (!event)
            continue;

        if (event->is<sf::Event::KeyPressed>()){
            auto key = event->getIf<sf::Event::KeyPressed>();
            if (key->code == sf::Keyboard::Key::Escape) {
                m_stop.request_stop();
            }
            else {
                m_messenger.publish<KEY>(key->code, true);
            }
        }

        if (event->is<sf::Event::KeyReleased>()){
            auto key = event->getIf<sf::Event::KeyReleased>();
            m_messenger.publish<KEY>(key->code, false);
        }

        if (event->is<sf::Event::MouseButtonPressed>()){
            auto mouse = event->getIf<sf::Event::MouseButtonPressed>();
            m_messenger.publish<CLICK>(
                mouse->position.x,
                mouse->position.y,
                true,
                mouse->button
            );
        }

        if (event->is<sf::Event::MouseButtonReleased>()){
            auto mouse = event->getIf<sf::Event::MouseButtonReleased>();
            m_messenger.publish<CLICK>(
                mouse->position.x,
                mouse->position.y,
                false,
                mouse->button
            );
        }

        if (event->is<sf::Event::MouseMoved>()){
            auto mouse = event->getIf<sf::Event::MouseMoved>();
            m_messenger.publish<MOUSE>(mouse->position.x, mouse->position.y);
        }
    }
}
