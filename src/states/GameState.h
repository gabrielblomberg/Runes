#pragma once

#include "Application.h"

#include "model/Runes.h"
#include "interface/Window.h"
#include "interface/Board.h"
#include "interface/RuneBox.h"

class GameState : public ApplicationState
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
    virtual std::unique_ptr<ApplicationState> main() override;

    /**
     * @brief Allows threads to join before destroying stop condition.
     */
    ~GameState() override {}; 

private:

    /**
     * @brief Handle a click.
     */
    void handle_click(const Message<CLICK> &click);

    /**
     * @brief Handle mouse movement.
     */
    void handle_mouse(const Message<MOUSE> &mouse);

    /**
     * @brief The game state thread used for rendering.
     */
    void render_thread();

    /// Mutex protecting concurrent access to the game state.
    std::mutex m_mutex;

    /// The dimensions of the screen in pixels.
    Vector2i m_screen_pixels;

    /// The game model.
    Runes m_runes;

    /// The of the game.
    Board m_board;

    /// Box of all the players runes.
    RuneBox m_box;

    /// The view of the thread.
    std::jthread m_render_thread;
};
