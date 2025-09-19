#pragma once

#include <stop_token>
#include <SFML/Window.hpp>

#include "utility/Messenger.h"
#include "utility/TypeList.h"

namespace Topic {

/**
 * @brief The user clicked on the screen.
 */
struct Click : public BroadcastTopic<Click>
{
    /**
     * @param x The pixel x coordinate.
     * @param y The pixel y coordinate.
     * @param pressed If the click was pressed or released.
     * @param button The button pressed (primary, secondary, etc).
     */
    struct Message {
        int x;
        int y;
        bool pressed;
        sf::Mouse::Button button;
    };
};

/**
 * @brief A key was pressed or released.
 */
struct Key : public BroadcastTopic<Key>
{
    /**
     * @param key The key pressed or released.
     * @param pressed True if pressed or false if released.
     */
    struct Message {
        sf::Keyboard::Key key;
        bool pressed;
    };
};

/**
 * @brief The mouse moved.
 */
struct Mouse : public BroadcastTopic<Mouse>
{
    /**
     * @param x The pixel x coordinate.
     * @param y The pixel y coordinate.
     */
    struct Message {
        int x;
        int y;
    };
};

} // namespace Topic

using Messaging = Messenger<
    TypeList::TypeList<
        Topic::Click,
        Topic::Key,
        Topic::Mouse
    >
>;
