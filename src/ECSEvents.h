#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window.hpp>

#include "system/logic/Runes.h"
#include "utility/SyncEventQueue.h"
#include "utility/TypeList.h"

namespace Event {
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

struct ScreenInfo {
    int width;
    int height;
};

using Connected = bool;
using PlaceRune = Runes::PlaceRune;
using AddPlayer = Runes::AddPlayer;
using GiveRune = Runes::GiveRune;
using MoveRune = Runes::MoveRune;

}  // namespace Event

enum EventId
{
    CLICK,
    KEY,
    MOUSE,
    SCREEN_INFO,
    CONNECTED,
    PLACE_RUNE,
    ADD_PLAYER,
    GIVE_RUNE,
    MOVE_RUN
};

using ECSEvents = SyncEventQueue<TypeList::TypeList<
    Event::Click,
    Event::Key,
    Event::Mouse,
    Event::Connected,
    Event::ScreenInfo,
    Event::PlaceRune,
    Event::AddPlayer,
    Event::GiveRune,
    Event::MoveRune>>;
