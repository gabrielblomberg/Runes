#pragma once

#include <stop_token>
#include <SFML/Window.hpp>

#include "utility/Messenger.h"
#include "utility/TypeList.h"
#include "system/RenderSystem.h"

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
    int button;
};

/**
 * @brief A event that a key was pressed or released.
 * 
 * @param key The key pressed or released.
 * @param pressed True if pressed or false if released.
 */
template<>
struct Event<KEY> {
    char key;
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

/**
 * @brief The event system responsible for handling and distributing events
 * throughout the system.
 */
class EventSystem
{
public:

    /**
     * @brief Create an event system.
     * @param render_system The render system to get the window to derive
     * operating system events from.
     * @param stop The stop token.
     */
    EventSystem(RenderSystem &render_system);

    /**
     * @brief Get the event system stop token.
     */
    inline std::stop_token get_stop() {
        return m_stop.get_token();
    }

    /**
     * @brief Run the event system.
     */
    void run();

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
    std::stop_source m_stop;

    /// The messenger for messages between different parts of the program.
    Messenger<Topics> m_messenger;
};
