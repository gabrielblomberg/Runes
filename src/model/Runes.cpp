#include "model/Runes.h"

using enum Runes::ActionType;

template<Runes::ActionType A>
using ActionData = Runes::ActionData<A>;

std::vector<Hexagon::Hexagon<int>> Runes::neighbors(Hexagon::Hexagon<int> hex)
{
    auto it = m_board.at(hex);
    if (it == m_board.end())
        return {};

    std::vector<Hexagon::Hexagon<int>> hexes;
    for (auto [key, vertex] : it.vertex().edges) {
        hexes.push_back(key);
    }

    return hexes;
}

bool Runes::connected()
{
    auto it = m_board.begin();
    if (it == m_board.end())
        return true;

    // Search through the whole graph, expect the search to not find the goal
    // but find a connected subgraph (hopefully the whole graph).
    auto s = DFS<Hexagon::Hexagon<int>>(
        [this](Hexagon::Hexagon<int> hex){ return neighbors(hex); },
        [](Hexagon::Hexagon<int> hex){ return false; }
    );

    s.perform(it.key());

    for (auto hex : m_board.keys()) {
        if (!s.visited().contains(hex))
            return false;
    }

    return true;
}

bool Runes::rune_moveable(Hexagon::Hexagon<int> hex)
{
    return false;
    // search::BFS<Hexagon::Hexagon<int>>();
}

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
    auto [it, success] = m_board.add_vertex(
        data.hexagon,
        Rune(data.rune, data.player_id)
    );

    // Add edges to the neighboring runes.
    for (auto &neighbor : data.hexagon.neighbors()) {
        if (m_board.contains_vertex(neighbor)) {
            m_board.add_edge(neighbor, data.hexagon);
            m_board.add_edge(data.hexagon, neighbor);
        }
    }

    return true;
}

template<>
bool Runes::action<MOVE_PLAYER_RUNE>(ActionData<MOVE_PLAYER_RUNE> &data)
{
    m_board.remove_vertex(data.from);
    return true;
}
