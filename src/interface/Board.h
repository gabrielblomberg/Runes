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
     * @param hexagon_size The size of the hexagons 
     */
    Board(Vector2i size, Vector2d hexagon_size);

    /**
     * @brief Get the pixel width and height of the board.
     * @return The pixel width and height of the board.
     */
    inline const Vector2i &size() {
        return m_size;
    }

    inline const Hexagon::Grid<Hexagon::GridType::FLAT> &grid() {
        return m_grid;
    }

    /**
     * @brief Draw an entire board to the window.
     * 
     * @param window 
     */
    void draw(Runes &runes);

    /**
     * @brief Draw a single hexagon to the window.
     */
    void draw_hexagon(
        Hexagon::Hexagon<int> hexagon,
        sf::Color colour = sf::Color::White
    );

    /**
     * @brief Add highlight to a hexagon.
     * 
     * @param hexagon The hexagon to add highlight to.
     * @param colour The colour of the highlight.
     */
    void add_highlight(Hexagon::Hexagon<int> hexagon, sf::Color colour)
    {
        m_highlights[hexagon] = colour;
    }

    /**
     * @brief Remove highlight from a hexagon.
     * 
     * @param hexagon The hexagon to remove highlight from.
     */
    inline void remove_highlight(Hexagon::Hexagon<int> hexagon)
    {
        m_highlights.erase(hexagon);
    }

    /**
     * @brief Display the board to a window.
     * 
     * @param window The window to display the board to.
     */
    void display(sf::RenderWindow &window);

private:

    /// The size of the board in pixels.
    Vector2i m_size;

    /// Hexagons to highlight.
    std::unordered_map<Hexagon::Hexagon<int>, sf::Color> m_highlights;

    /// The texture to draw the board onto, that is then drawn to the window.
    sf::RenderTexture m_texture;

    /// Hexagonal grid to play on.
    Hexagon::Grid<Hexagon::GridType::FLAT> m_grid;

    /// View of the board.
    sf::View m_view;

    /// Hexagon to draw.
    sf::ConvexShape m_hexagon;
};
