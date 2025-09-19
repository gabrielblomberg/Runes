#include "system/interface/InterfaceSystem.h"

#include "system/interface/view/Game.h"
#include "system/render/RenderSystem.h"
#include "system/MessagingSystem.h"

InterfaceSystem::InterfaceSystem(
    RenderSystem *render_system,
    MessagingSystem *event_system,
    std::stop_token stop
  ) : m_stop_source(stop)
    , m_render_system(render_system)
    , m_event_system(event_system)
{}

void InterfaceSystem::main()
{
    m_interface = std::make_unique<GameInterface>(*this);
    while (m_interface && !m_stop_source) {
        m_interface = state->run(StopCondition(m_stop_source.get_token()));
    }
}

void InterfaceSystem::start()
{
    if (!m_thread) {
        m_thread = std::jthread(&InterfaceSystem::main, this)
    }
}

void InterfaceSystem::handle_input()
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
