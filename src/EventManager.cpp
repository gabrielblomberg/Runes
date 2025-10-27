#include "EventManager.h"
#include <SFML/Window/Event.hpp>

void EventManager::step()
{
    for(;;) {
        auto event = m_window->pollEvent();
        if (!event || m_stop.stop_requested())
            return

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
}
