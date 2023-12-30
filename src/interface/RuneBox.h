#pragma once

#include <optional>
#include<initializer_list>

#include <SFML/Graphics.hpp>

#include "model/Runes.h"
#include "util/Hexagon.h"
#include "util/Vector2.h"

/**
 * @brief A collection of runes.
 * 
 */
class RuneBox
{
public:

    /**
     * @brief Create a new rune box.
     *  
     * @param dimensions The dimensions of the rune box in pixels.
     * @param hexagon_size The radius of the hexagon in pixels.
     */
    RuneBox(Vector2i dimensions, int hexagon_size);

    /**
     * @brief Create a new rune box containing runes.
     * 
     * @param dimensions The dimensions of the rune box in pixels.
     * @param hexagon_size The width and height of the hexagon in pixels.
     * @param runes The initial set of runes in the box.
     */
    RuneBox(
        Vector2i dimensions,
        Vector2i hexagon_size,
        std::initializer_list<Runes::RuneType> runes
    );

    /**
     * @brief Add a rune to the box.
     * 
     * @param type The type of rune to add.
     */
    bool add(Runes::RuneType type);

    /**
     * @brief Remove a rune from the box.
     * 
     * @param type 
     */
    void remove(Runes::RuneType type);

    /**
     * @brief Get a rune at a position in the box.
     * 
     * @param position The pixel position in the box, relative to the bottom
     * left of the box.
     * 
     * @return The rune at the position or std::nullopt if no rune exists at
     * that location.
     */
    std::optional<Runes::RuneType> get(Vector2i position);

    /**
     * @brief Draw the rune box onto a render target.
     * 
     * @param target The target to draw the box onto.
     */
    void draw(sf::RenderTarget &target, Vector2i position);

private:

    /// The dimensions of the box in pixels.
    Vector2i m_radius;

    // The radius of the hexagon in pixels.
    int m_hexagon_diameter;

    /// The maximum tiles in each row.
    Vector2i m_spaces;

    /// The spaces that runes are allowed to be located.
    std::size_t m_maximum;

    /// All the tiles in the box.
    std::vector<Runes::RuneType> m_tiles;

    /// The hexagon shape draw.
    sf::ConvexShape m_hexagon;
};