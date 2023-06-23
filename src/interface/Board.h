#pragma once

#include <SFML/Graphics.hpp>

#include "interface/Window.h"
#include "util/Hexagon.h"
#include "util/Vector2.h"
#include "model/Runes.h"

class Board
{
public:

    /**
     * @brief Create a new view of the board.
     * 
     * @param size The pixel width and height of the board.
     * @param position The pixel (x, y) position of the board.
     * @param hexagon_size The size of the hexagons 
     */
    Board(
        Vector2i size,
        Vector2i position,
        Vector2d hexagon_size
    );

    inline const Vector2i &size() {
        return m_size;
    }

    inline const Vector2i &position() {
        return m_position;
    }

    inline const Hexagon::Grid<Hexagon::GridType::FLAT> &grid() {
        return m_grid;
    }

    /**
     * @brief Draw an entire board to the window.
     * 
     * @param window 
     */
    void draw(Window &window, Runes &runes);

    /**
     * @brief Draw a single hexagon to the window.
     */
    void draw_hexagon(Hexagon::Hexagon<int> hexagon);

    /**
     * @brief Display the board to a window.
     * 
     * @param window The window to display the board to.
     */
    void display(Window &window);

private:

    /// The size of the board in pixels.
    Vector2i m_size;

    /// The position of the board in pixels from the top left of the screen.
    Vector2i m_position;

    /// The texture to draw the board onto, that is then drawn to the window.
    sf::RenderTexture m_texture;

    /// Hexagonal grid to play on.
    Hexagon::Grid<Hexagon::GridType::FLAT> m_grid;

    /// View of the board.
    sf::View m_view;

    /// Hexagon to draw.
    sf::ConvexShape m_hexagon;
};
