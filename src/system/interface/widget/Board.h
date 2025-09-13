#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include "system/render/RenderSystem.h"
#include "system/EntitySystem.h"
#include "utility/Hexagon.h"
#include "utility/Vector2.h"
#include "system/logic/Runes.h"

class Board
{
public:

    /**
     * @brief Initialise a new board.
     * 
     * @param entity The entity of the board.
     * @param size The pixel width and height of the board.
     * @param hexagon_size The size of the hexagons 
     */
    Board(Runes &runes, Vector2i size, Vector2d hexagon_size);

    /**
     * @brief Get the grid.
     */
    inline Hexagon::Grid<Hexagon::GridType::FLAT> &grid() {
        return m_grid;
    }

    inline Entity entity() {
        return m_entity;
    }

    /**
     * @brief Add highlight to a hexagon.
     * 
     * @param hexagon The hexagon to add highlight to.
     * @param colour The colour of the highlight.
     */
    void add_highlight(Hexagon::Hexagon<int> hexagon, sf::Color colour)
    {
        std::unique_lock lock(m_mutex);
        m_highlights[hexagon] = colour;
    }

    /**
     * @brief Remove highlight from a hexagon.
     * 
     * @param hexagon The hexagon to remove highlight from.
     */
    inline void remove_highlight(Hexagon::Hexagon<int> hexagon)
    {
        std::unique_lock lock(m_mutex);
        m_highlights.erase(hexagon);
    }

private:

    /**
     * @brief Display the board to a window.
     * @param window The window to display the board to.
     */
    void render(RenderLock &renderer);

    /**
     * @brief Draw a single hexagon to the window.
     */
    void render_hexagon(
        Hexagon::Hexagon<int> hexagon,
        sf::Color colour = sf::Color::White
    );

    /// The entity of the board.
    Entity m_entity;

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

    Runes *m_runes;

    std::mutex m_mutex;
};
