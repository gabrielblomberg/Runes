#pragma once

#include <thread>

#include "core/Components.h"
#include "core/EventSystem.h"
#include "core/RenderSystem.h"
#include "util/StopCondition.h"

class Game
{
public:

    /**
     * @brief An game state, such as main menu or game screen.
     */
    class State
    {
    public:

        /**
         * @brief Construct the game state.
         * 
         * @param game Pointer to the owning game.
         */
        inline State(Game *game)
            : m_game(game)
        {}

        /**
         * @brief Virtual destructor for inheritance.
         */
        inline virtual ~State() {};

        /**
         * @brief Main method to run the state.
         * 
         * @param stop The stop token to stop on.
         * 
         * @return The next state, or nullptr if none.
         */
        virtual std::vector<std::unique_ptr<State>> run(StopCondition &&stop) = 0;

    protected:

        // Pointer to the owning game.
        Game *m_game;
    };

    /**
     * @brief Initialise the game.
     */
    Game();

    /**
     * @brief The main thread that handles input events.
     */
    inline void main() {
        m_event_system.run();
    }

    /**
     * @brief The thread controlling the logic of the application.
     * @param stop Stop signal to exit.
     */
    void state_machine(std::stop_token stop);

    /**
     * @brief Get a reference to the messenger.
     */
    inline auto &messenger() {
        return m_event_system.messenger();
    }

    /**
     * @brief Get a reference to the window.
     */
    inline RenderSystem &renderer() {
        return m_renderer;
    }

private:

    /// Entity component system.
    ECS m_ecs;

    /// The window that states can draw to.
    RenderSystem m_renderer;

    /// The event system.
    EventSystem m_event_system;

    /// State of the game.
    std::vector<std::unique_ptr<State>> m_states;

    /// Thread running the game.
    std::jthread m_state_thread;
};
