#pragma once

#include <stop_token>
#include <SFML/Window.hpp>

#include "utility/Messenger.h"
#include "utility/TypeList.h"

/**
 * @brief Messages sent in the program.
 */
enum EventType {
    CLICK,
    KEY,
    MOUSE
};

/**
 * @brief Base event structure type that is specialised based on the
 * EventType enumeration.
 */
template<EventType>
struct Event;

/**
 * @brief An event that the user clicked on a pixel on the current screen.
 * 
 * @param x The pixel x coordinate.
 * @param y The pixel y coordinate.
 * @param pressed If the click was pressed or released.
 * @param button The button pressed (primary, secondary, etc).
 */
template<>
struct Event<CLICK> {
    int x;
    int y;
    bool pressed;
    sf::Mouse::Button button;
};

/**
 * @brief A event that a key was pressed or released.
 * 
 * @param key The key pressed or released.
 * @param pressed True if pressed or false if released.
 */
template<>
struct Event<KEY> {
    sf::Keyboard::Key key;
    bool pressed;
};

/**
 * @brief The mouse moved to a location.
 * 
 * @param x The pixel x coordinate.
 * @param y The pixel y coordinate.
 */
template<>
struct Event<MOUSE> {
    int x;
    int y;
};

/**
 * @brief Topics available for messaging.
 */
using Topics = TypeList::TypeList<
    Event<CLICK>,
    Event<KEY>,
    Event<MOUSE>
>;

using Messaging = Messenger<Topics>;

/**
 * @brief The event system responsible for handling and distributing events
 * throughout the system.
 */
class MessagingSystem
{
public:

    /**
     * @brief Create an event system.
     * @param render_system The render system to get the window to derive
     * operating system events from.
     * @param stop The stop token.
     */
    MessagingSystem(sf::Window *window, std::stop_source &&stop_source);

    /**
     * @brief Run the event system.
     */
    void main();

    /**
     * @brief Get the event system messenger.s
     */
    inline Messenger<Topics> &messenger() {
        return m_messenger;
    }

private:

    /// Pointer to the window to handle operating system events from.
    sf::Window *m_window;

    /// Source to stop.
    std::stop_source m_stop_source;

    /// The messenger for messages between different parts of the program.
    Messenger<Topics> m_messenger;
};
