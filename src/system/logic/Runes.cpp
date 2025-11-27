#include "Runes.h"

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

std::optional<std::size_t> Runes::add_player(const AddPlayer& request)
{
    auto player_id = m_players.size();
    m_players.emplace_back(player_id, request.name);
    return player_id;
}

bool Runes::give_rune(const GiveRune &request)
{
    if (m_players.size() > request.player_id)
        return false;

    // Find the runes.
    auto &runes = m_players[request.player_id].m_runes;
    auto it = runes.find(request.rune);

    if (it == runes.end())
        runes[request.rune] = request.n;
    else
        runes[request.rune] += request.n;

    return true;
}

bool Runes::place_rune(const PlaceRune &request)
{
    auto [it, success] = m_board.add_vertex(
        request.hexagon,
        Rune(request.rune, request.player_id)
    );

    // Add edges to the neighboring runes.
    for (auto &neighbor : request.hexagon.neighbors()) {
        if (m_board.contains_vertex(neighbor)) {
            m_board.add_edge(neighbor, request.hexagon);
            m_board.add_edge(request.hexagon, neighbor);
        }
    }

    return true;
}

bool Runes::move_rune(const MoveRune &request)
 {
    m_board.remove_vertex(request.from);
    return true;
}
