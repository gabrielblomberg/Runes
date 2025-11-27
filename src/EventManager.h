#pragma once

#include <stop_token>
#include <SFML/Window.hpp>

#include "utility/TypeList.h"
#include "utility/Topic.h"
#include "system/logic/Runes.h"

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

    struct ScreenInfo {
        int width;
        int height;
    };

    using Connected = bool;
    using PlaceRuneRequest = Runes::PlaceRune;
    using AddPlayerRequest = Runes::AddPlayer;
    using GiveRuneRequest = Runes::GiveRune;
    using MoveRuneRequest = Runes::MoveRune;

} // namespace Message

namespace Topic {

using Click = BroadcastTopic<Message::Click>;
using Key = BroadcastTopic<Message::Key>;
using Mouse = BroadcastTopic<Message::Mouse>;
using Connected = BroadcastTopic<Message::Connected>;
using Screen = MemoryTopic<Message::ScreenInfo>;
using PlaceRune = RequestTopic<Runes::PlaceRune, bool>;
using AddPlayer = RequestTopic<Runes::AddPlayer, bool>;
using GiveRune = RequestTopic<Runes::GiveRune, bool>;
using MoveRune = RequestTopic<Runes::MoveRune, bool>;

} // namespace Topic

using Topics = TypeList::TypeList<
    Topic::Click,
    Topic::Key,
    Topic::Mouse,
    Topic::Connected,
    Topic::Screen,
    Topic::PlaceRune,
    Topic::AddPlayer,
    Topic::GiveRune,
    Topic::MoveRune
>;

class EventManager : public Messenger<Topics>
{
public:

    EventManager(sf::Window *window, std::stop_source &&stop_source)
        : Messenger<Topics>(stop_source.get_token())
        , m_stop(stop_source)
        , m_window(window)
    {}

    void step();

private:
    
    /// Source to stop.
    std::stop_source m_stop;
    
    /// Pointer to the window to handle operating system events from.
    sf::Window *m_window;
};
