#pragma once

#include <functional>

#include "Game.h"
#include "model/Runes.h"

#include "interface/Board.h"

class GameState : public Game::State
{
public:

    /**
     * @brief Instantiate a new game state.
     */
    GameState(Game *app);

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

    /// The dimensions of the screen in pixels.
    Vector2i m_screen_pixels;

    /// The game model.
    Runes m_runes;

    /// The of the game.
    Board m_board;
};
