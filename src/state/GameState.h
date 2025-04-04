#pragma once

#include <functional>

#include "Game.h"
#include "Runes.h"

#include "object/Board.h"

class GameState : public Game::State
{
public:

    /**
     * @brief Instantiate a new game state.
     */
    GameState(Game &app);

    /**
     * @brief Runs the main game state.
     * @return Nullptr indicating game exit.
     */
    virtual std::vector<std::unique_ptr<Game::State>> run(StopCondition &&stop) override;

    /**
     * @brief Allows threads to join before destroying stop condition.
     */
    virtual ~GameState() {}; 

private:

    /**
     * @brief Handle a click.
     */
    void handle_click(const Event<CLICK> &click);

    /**
     * @brief Handle mouse movement.
     */
    void handle_mouse(const Event<MOUSE> &mouse);

    /// Mutex protecting concurrent access to the game state.
    std::mutex m_mutex;

    /// The scene of the game state.
    Scene m_scene;

    /// The game model.
    Runes m_runes;

    /// The board.
    Board m_board;
};
