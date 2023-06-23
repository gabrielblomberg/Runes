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
