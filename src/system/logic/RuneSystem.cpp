#include "system/logic/RuneSystem.h"

RuneSystem::RuneSystem(ECS *ecs, EventManager *events, std::stop_token stop)
    : System(ecs, events, stop)
{
    // m_subscriptions.push_back(events->add_player.subscribe(
    //     [&](const auto &arg){ return m_runes.add_player(arg); }
    // ));

    m_subscriptions.push_back(events->give_rune.subscribe(
        [&](const auto &arg){ return m_runes.give_rune(arg); }
    ));

    m_subscriptions.push_back(events->place_rune.subscribe(
        [&](const auto &arg){ return m_runes.place_rune(arg); }
    ));

    m_subscriptions.push_back(events->move_rune.subscribe(
        [&](const auto &arg){ return m_runes.move_rune(arg); }
    ));
}
