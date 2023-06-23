#pragma once

#include "util/Messenger.h"

/**
 * @brief Messages sent in the program.
 */
enum MessageType {
    CLICK
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
 */
template<>
struct Message<CLICK> {
    int x;
    int y;
};

/**
 * @brief Topics available for messaging.
 */
using Topics = TypeList::TypeList<
    Message<CLICK>
>;
