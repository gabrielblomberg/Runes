#include "interface/RuneBox.h"

RuneBox::RuneBox(Vector2i dimensions, int hexagon_diameter)
    : m_radius(dimensions / 2)
    , m_hexagon_diameter(hexagon_diameter)
    , m_spaces(
        (m_radius.x / (m_hexagon_diameter) - 1),
        (m_radius.y / (m_hexagon_diameter) - 1)
    )
{
    m_maximum = m_spaces.x * m_spaces.y;

    // Define the hexagonal grid to have the same size d
    auto grid = Hexagon::Grid<Hexagon::GridType::POINTY>(
        hexagon_diameter,
        hexagon_diameter,
        0.0,
        0.0
    );

    // Create a hexagon that will be drawn.
    m_hexagon.setPointCount(6);
    for (int i = 0; i < 6; i++) {
        auto [x, y] = grid.corner_offset(i);
        m_hexagon.setPoint(i, sf::Vector2f(x, y));
    }
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

void RuneBox::draw(sf::RenderTarget &target, Vector2i position)
{
    auto background = sf::RectangleShape(m_radius * 2.0);
    background.setFillColor(sf::Color::Black);
    background.setOutlineColor(sf::Color::White);
    background.setPosition(position.x - m_radius.x, position.y - m_radius.y);
    background.setOutlineThickness(2);
    target.draw(background);

    for (std::size_t index = 0; index < m_tiles.size(); index++) {

        int i = index % m_spaces.x;
        int j = index / m_spaces.x;

        int x = position.x - m_radius.x + (
            1.5 * m_hexagon_diameter + // Left padding
            i * (2 * m_hexagon_diameter + m_hexagon_diameter / m_spaces.x)
        );

        int y = position.y - m_radius.y + (
            1.5 * m_hexagon_diameter + // Top padding
            j * (2 * m_hexagon_diameter + m_hexagon_diameter / m_spaces.y)
        );

        m_hexagon.setPosition(x, y);
        m_hexagon.setFillColor(sf::Color::White);
        m_hexagon.setOutlineColor(sf::Color::Black);
        m_hexagon.setOutlineThickness(2);
        target.draw(m_hexagon);
    }
}
