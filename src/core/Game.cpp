#include "Game.h"

#include "core/GameState.h"
#include "util/StopCondition.h"

Game::Game()
    : m_renderer(m_ecs, "Runes")
    , m_event_system(m_renderer)
{
    m_renderer.start(m_event_system.get_stop());
    m_state_thread = std::jthread(&Game::state_machine, this, m_event_system.get_stop());
}

void Game::state_machine(std::stop_token stop)
{
    std::unique_ptr<State> state = std::make_unique<GameState>(this);

    while (state) {
        auto extend = state->run(StopCondition(stop));

        for (auto &state : extend)
            m_states.push_back(std::move(state));

        if (!m_states.empty()) {
            state = std::move(m_states.back());
            m_states.pop_back();
            continue;
        }

        state = nullptr;
        break;
    }
}
