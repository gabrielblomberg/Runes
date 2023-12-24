#include "model/Runes.h"

using enum Runes::ActionType;

template<Runes::ActionType A>
using ActionData = Runes::ActionData<A>;

template<>
bool Runes::action<ADD_PLAYER>(ActionData<ADD_PLAYER> &data)
{
    data.player_id = m_players.size();
    m_players.emplace_back(data.player_id, data.name);

    return true;
}

template<>
bool Runes::action<GIVE_PLAYER_RUNE>(ActionData<GIVE_PLAYER_RUNE> &data)
{
    if (m_players.size() > data.player_id)
        return false;

    // Find the runes.
    auto &runes = m_players[data.player_id].m_runes;
    auto it = runes.find(data.rune);

    if (it == runes.end()) {
        runes[data.rune] = data.n;
    }
    else {
        runes[data.rune] += data.n;
    }

    return true;
}

template<>
bool Runes::action<PLACE_PLAYER_RUNE>(ActionData<PLACE_PLAYER_RUNE> &data)
{
    auto [it, success] = m_map.add_vertex(data.hexagon, Rune(RuneType::VITALITY, 0));
    if (!success)
        return false;

    m_map.add_edge(m_previous, data.hexagon);
    m_previous = data.hexagon;
    return true;
}

template<>
bool Runes::action<MOVE_PLAYER_RUNE>(ActionData<MOVE_PLAYER_RUNE> &data)
{
    m_map.remove_vertex(data.from);
    m_previous = data.from;
    return true;
}
