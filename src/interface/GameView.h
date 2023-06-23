#pragma once

#include <stop_token>

#include "interface/Window.h"
#include "interface/Board.h"
#include "interface/Button.h"
#include "model/Runes.h"
#include "util/Vector2.h"

class GameView
{
public:

    /**
     * @brief Create a new view of the game.
     */
    GameView(Runes &runes, Window &window);

    /**
     * @brief Displays the game to a window.
     * 
     * @param window The window to display the game to.
     */
    void display(Window &window);

    /**
     * @brief Handle a click on the game interface.
     * 
     * @param pixel 
     * @returns A game action if an action results from the interaction.
     */
    void handle_click(int x, int y);

    /**
     * @brief Handle a keyboard press on the game interface.
     * 
     * @param key The key being pressed.
     * @returns A game action if an action results from the interaction.
     */
    void handle_key(sf::Keyboard::Key key);

private:

    /// Reference to the game to display.
    Runes &m_runes;

    /// The game board.
    Board m_board;
};
