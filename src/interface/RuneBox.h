#pragma once

#include <optional>
#include<initializer_list>

#include <SFML/Graphics.hpp>

#include "model/Runes.h"
#include "util/Hexagon.h"
#include "util/Eigen.h"

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
     * @param position The centre position to draw the box on the screen.
     */
    RuneBox(
        Vector2i dimensions,
        int hexagon_size,
        Vector2i position = Vector2i{0, 0}
    );

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
     * @brief Set the dimensions and update the rune box capacity with the new
     * size.
     * 
     * @param dimensions The pixel width and height of the rune box.
     */
    void set_dimensions(Vector2i dimensions);

    /**
     * @brief Set the centre position of the rune box.
     * 
     * @param centre The pixel centre of the box.
     */
    void set_position(Vector2i centre);

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
    void draw(sf::RenderTarget &target);

private:

    inline Vector2i to_pixel(std::size_t index) {
        return (
            m_centre - m_radius +
            Vector2i((int)index % m_spaces.x(), (int)index / m_spaces.x()) / m_spaces * // Index
            3.5 * Vector2i(m_hexagon_diameter, m_hexagon_diameter) + // Positioning.
        );
    }

    // inline std::size_t to_rune(Vector2i position) {
    //     return (
    //         position / (2 * m_hexagon_diameter) - m_hexagon_diameter)
    //         / (1.5 * m_hexagon_diameter)
    //         + m_radius - m_centre
    //     );
    // }

    /// The centre position of the box.
    Vector2i m_centre;

    /// The dimensions of the box in pixels.
    Vector2i m_radius;

    /// The maximum tiles in each row.
    Vector2i m_spaces;

    /// The spaces that runes are allowed to be located.
    std::size_t m_maximum;

    /// All the tiles in the box.
    std::vector<Runes::RuneType> m_tiles;

    // The radius of the hexagon in pixels.
    int m_hexagon_diameter;

    /// The hexagon shape draw.
    sf::ConvexShape m_hexagon;

    // The background of the box.
    sf::RectangleShape m_background;
};