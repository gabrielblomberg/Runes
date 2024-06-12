#include "interface/RuneBox.h"

RuneBox::RuneBox(Vector2i dimensions, int hexagon_diameter, Vector2i centre)
    : m_hexagon_diameter(hexagon_diameter)
{
    set_dimensions(dimensions);
    set_position(centre);

    // Define the hexagonal grid to have the same size d
    auto grid = Hexagon::Grid<Hexagon::GridType::POINTY>(
        hexagon_diameter,
        hexagon_diameter,
        0.0,
        0.0
    );

    // Create a hexagon that will be drawn.
    m_hexagon.setPointCount(6);
    m_hexagon.setFillColor(sf::Color::White);
    m_hexagon.setOutlineColor(sf::Color::Black);
    m_hexagon.setOutlineThickness(2);

    for (int i = 0; i < 6; i++) {
        auto [x, y] = grid.corner_offset(i);
        m_hexagon.setPoint(i, sf::Vector2f(x, y));
    }

    m_background = sf::RectangleShape(dimensions);
    m_background.setFillColor(sf::Color::Black);
    m_background.setOutlineColor(sf::Color::White);
    m_background.setOutlineThickness(2);
    m_background.setPosition(m_centre.x - m_radius.x, m_centre.y - m_radius.y);
}

bool RuneBox::add(Runes::RuneType type)
{
    if (m_tiles.size() + 1 > m_maximum)
        return false;

    m_tiles.emplace_back(type);
    return true;
}

void RuneBox::remove(Runes::RuneType type)
{
    auto it = std::find_if(
        m_tiles.begin(),
        m_tiles.end(),
        [type](const Runes::RuneType &tile){ return tile == type; }
    );

    if (it == m_tiles.end())
        return;
 
    m_tiles.erase(it);
}

std::optional<Runes::RuneType> RuneBox::get(Vector2i position)
{
    return std::nullopt;
}

void RuneBox::draw(sf::RenderTarget &target)
{
    target.draw(m_background);

    for (std::size_t index = 0; index < m_tiles.size(); index++) {
        Vector2i pixel = to_pixel(index);
        m_hexagon.setPosition(pixel.x, pixel.y);
        target.draw(m_hexagon);
    }
}

void RuneBox::set_dimensions(Vector2i dimensions)
{
    m_radius = dimensions / 2;
    m_background.setPosition(m_centre.x - m_radius.x, m_centre.y - m_radius.y);
    m_background.setSize(dimensions);

    m_spaces = Vector2i{
        (m_radius.x / (m_hexagon_diameter) - 1),
        (m_radius.y / (m_hexagon_diameter) - 1)
    };

    m_maximum = m_spaces.x * m_spaces.y;
}

void RuneBox::set_position(Vector2i centre)
{
    m_centre = centre;
    m_background.setPosition(m_centre.x - m_radius.x, m_centre.y - m_radius.y);
}
