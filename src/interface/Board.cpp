#include "interface/Board.h"

Board::Board(
        Vector2i size,
        Vector2i position,
        Vector2d hexagon_size
  ) : m_size(size)
    , m_position(position)
    , m_texture()
    , m_grid()
    , m_view()
    , m_hexagon()
{
    // Options for the texture storing the hexagonal grid.
    sf::ContextSettings texture_settings;
    texture_settings.antialiasingLevel = 8;

    // Create the texture that contains the board.
    if (!m_texture.create(size.x, size.y, texture_settings))
        throw std::runtime_error("Failed to create runes texture.");

    // Define the transformation from the texture to the window. The texture has
    // the given size and should be drawn in the middle of the screen.
    m_view.setSize(m_size.x, m_size.y);
    m_view.setCenter(m_size.x / 2, m_size.y / 2);

    // Define the hexagonal grid to have the same size d
    m_grid = Hexagon::Grid(
        hexagon_size.x,
        hexagon_size.y,
        size.x / 2,
        size.y / 2
    );

    // Create a hexagon that will be drawn.
    m_hexagon.setPointCount(6);
    for (int i = 0; i < 6; i++) {
        auto [x, y] = m_grid.corner_offset(i);
        m_hexagon.setPoint(i, sf::Vector2f(x, y));
    }
}

void Board::draw(Window &window, Runes &runes)
{
    m_hexagon.setFillColor(sf::Color::Black);
    m_hexagon.setOutlineColor(sf::Color::White);
    m_hexagon.setOutlineThickness(1);
    m_texture.setView(m_view);

    for (int x = 0; x < m_size.x; x += 50) {
        for (int y = 0; y < m_size.y; y += 50) {
            m_hexagon.setPosition(x, y);
            m_texture.draw(m_hexagon);
        }
    }

    display(window);
}

void Board::draw_hexagon(Hexagon::Hexagon<int> hexagon)
{
    auto [x, y] = m_grid.to_pixel(hexagon);

    m_hexagon.setPosition(x, y);
    m_hexagon.setFillColor(sf::Color::White);
    m_hexagon.setOutlineColor(sf::Color::Black);
    m_hexagon.setOutlineThickness(2);

    m_texture.setView(m_view);
    m_texture.draw(m_hexagon);
}

void Board::display(Window &window)
{
    m_texture.display();

    sf::Sprite sprite;
    sprite.setTexture(m_texture.getTexture());
    window->setView(m_view);
    window->draw(sprite);
}
