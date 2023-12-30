#pragma once

#include "util/TypeList.h"

/**
 * @brief Messages sent in the program.
 */
enum MessageType {
    CLICK,
    KEY,
    MOUSE
};

/**
 * @brief Base message structure type that is specialised based on the
 * MessageType enumeration.
 */
template<MessageType>
struct Message;

/**
 * @brief A message that the user clicked on a pixel on the current screen.
 * 
 * @param x The pixel x coordinate.
 * @param y The pixel y coordinate.
 * @param pressed If the click was pressed or released.
 * @param button The button pressed (primary, secondary, etc).
 */
template<>
struct Message<CLICK> {
    int x;
    int y;
    bool pressed;
    int button;
};

/**
 * @brief A message that a key was pressed or released.
 * 
 * @param key The key pressed or released.
 * @param pressed True if pressed or false if released.
 */
template<>
struct Message<KEY> {
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
struct Message<MOUSE> {
    int x;
    int y;
};

/**
 * @brief Topics available for messaging.
 */
using Topics = TypeList::TypeList<
    Message<CLICK>,
    Message<KEY>,
    Message<MOUSE>
>;
