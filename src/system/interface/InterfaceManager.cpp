#include "system/interface/InterfaceSystem.h"

#include "system/interface/view/Game.h"
#include "system/render/RenderSystem.h"
#include "system/MessagingSystem.h"

InterfaceSystem::InterfaceSystem(
    ECS *ecs,
    EventManager *events,
    std::stop_token stop
  ) : m_stop(stop)
    , m_ecs(ecs)
    , m_events(events)
{}

void InterfaceSystem::step()
{
    m_interface = std::make_unique<GameInterface>(*this);
    while (!m_stop.stop_requested())
    while (m_interface && !m_stop_source) {
        m_interface = state->run(StopCondition(m_stop_source.get_token()));
    }
}

void InterfaceManager::process_input()
{
    auto event = m_window->pollEvent();
    if (!event)
        continue;

    if (auto key = event->getIf<sf::Event::KeyPressed>()) {
        this->key.publish(key->code, true);
    }
    else if (auto key = event->getIf<sf::Event::KeyReleased>()) {
        this->key.publish(key->code, false);
    }
    else if (auto mouse = event->getIf<sf::Event::MouseButtonPressed>()){
        this->click.publish(
            Message::Click {
                .x = mouse->position.x,
                .y = mouse->position.y,
                .pressed = true,
                .button = mouse->button
            }
        );
    }
    else if (auto mouse = event->getIf<sf::Event::MouseButtonReleased>()){
        this->click.publish(
            Message::Click {
                .x = mouse->position.x,
                .y = mouse->position.y,
                .pressed = false,
                .button = mouse->button
            }
        );
    }
    else if (auto mouse = event->getIf<sf::Event::MouseMoved>()){
        this->mouse.publish(mouse->position.x, mouse->position.y);
    }
}