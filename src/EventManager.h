#pragma once

#include <stop_token>
#include <SFML/Window.hpp>

#include "utility/TypeList.h"
#include "utility/Topic.h"

namespace Message
{
    /**
     * @param x The pixel x coordinate.
     * @param y The pixel y coordinate.
     * @param pressed If the click was pressed or released.
     * @param button The button pressed (primary, secondary, etc).
     */
    struct Click {
        int x;
        int y;
        bool pressed;
        sf::Mouse::Button button;
    };

    /**
     * @param key The key pressed or released.
     * @param pressed True if pressed or false if released.
     */
    struct Key {
        sf::Keyboard::Key key;
        bool pressed;
    };

    /**
     * @param x The pixel x coordinate.
     * @param y The pixel y coordinate.
     */
    struct Mouse {
        int x;
        int y;
    };

} // namespace Message

namespace Topic {

using Click = BroadcastTopic<Message::Click>;
using Key = BroadcastTopic<Message::Key>;
using Mouse = BroadcastTopic<Message::Mouse>;

} // namespace Topic

class EventManager
{
public:

    EventSystem(sf::Window *window, std::stop_source &&stop_source)
        : m_window(window)
        , click(stop)
        , key(stop)
        , mouse(stop)
    {}

private:

    void step();

    /// Source to stop.
    std::stop_source  m_stop;

    /// Pointer to the window to handle operating system events from.
    sf::Window *m_window;

public:

    Topic::Click click;
    Topic::Key key;
    Topic:::Mouse mouse;
};
