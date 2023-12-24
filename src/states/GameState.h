#pragma once

#include "Application.h"

#include "model/Runes.h"
#include "interface/Window.h"
#include "interface/Board.h"

class GameState : public State
{
public:

    /**
     * @brief Instantiate a new game state.
     */
    GameState(Application *app, std::stop_token stop);

    /**
     * @brief Runs the main game state.
     * @return Nullptr indicating application exit.
     */
    virtual std::unique_ptr<State> main() override; 

private:

    /**
     * @brief Handle a click.
     */
    void handle_click(const Message<CLICK> &click);

    /// The game model.
    Runes m_runes;

    /// The of the game.
    Board m_board;

    /// The view of the thread.
    std::jthread m_view_thread;
};
