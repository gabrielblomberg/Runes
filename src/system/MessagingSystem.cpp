#include "system/MessagingSystem.h"

MessagingSystem::MessagingSystem(sf::Window *window, std::stop_source &&stop_source)
    : m_window(window)
    , m_stop_source(stop_source)
    , m_messenger()
{}

void MessagingSystem::main()
{
    while (!m_stop_source.stop_requested())
    {
        auto event = m_window->waitEvent();
        if (!event)
            continue;

        if (event->is<sf::Event::KeyPressed>()){
            auto key = event->getIf<sf::Event::KeyPressed>();
            if (key->code == sf::Keyboard::Key::Escape) {
                m_stop_source.request_stop();
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
