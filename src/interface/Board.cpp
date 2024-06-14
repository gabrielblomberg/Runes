#include "interface/Board.h"

Board::Board(Vector2d size, Vector2d hexagon_size)
    : m_size(size)
    , m_texture()
    , m_grid()
    , m_view()
    , m_hexagon()
{
    // Options for the texture storing the hexagonal grid.
    sf::ContextSettings texture_settings;
    texture_settings.antialiasingLevel = 8;

    // Create the texture that contains the board.
    if (!m_texture.create(size.x(), size.y(), texture_settings))
        throw std::runtime_error("Failed to create runes texture.");

    // Define the transformation from the texture to the window. The texture has
    // the given size and should be drawn in the middle of the screen.
    m_view.setSize(m_size.x(), m_size.y());
    m_view.setCenter(m_size.x() / 2, m_size.y() / 2);

    // Define the hexagonal grid to have the same size d
    m_grid = Hexagon::Grid<Hexagon::GridType::FLAT>(
        hexagon_size.x(),
        hexagon_size.y(),
        size.x() / 2,
        size.y() / 2
    );

    // Create a hexagon that will be drawn.
    m_hexagon.setPointCount(6);
    for (int i = 0; i < 6; i++) {
        auto [x, y] = m_grid.corner_offset(i);
        m_hexagon.setPoint(i, sf::Vector2f(x, y) * 0.95);
    }
}

void Board::draw(Runes &runes)
{
    m_texture.clear();
    if (runes.connected()) {
        m_hexagon.setOutlineColor(sf::Color::White);
    }
    else {
        m_hexagon.setOutlineColor(sf::Color::Red);
    }

    m_hexagon.setFillColor(sf::Color::Black);
    m_hexagon.setOutlineThickness(1);
    m_texture.setView(m_view);

    for (const auto &[a, vertex] : runes.board().vertices()) {
        draw_hexagon(a);
        auto [x0, y0] = m_grid.to_pixel(a);

        for (const auto &[b, edge] : vertex->edges) {
            auto [x1, y1] = m_grid.to_pixel(b);

            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(x0, y0)),
                sf::Vertex(sf::Vector2f(x1, y1))
            };

            m_texture.draw(line, 2, sf::Lines);
        }
    }

    for (auto &[hex, colour] : m_highlights)
        draw_hexagon(hex, colour);
}

void Board::draw_hexagon(Hexagon::Hexagon<int> hexagon, sf::Color colour)
{
    auto [x, y] = m_grid.to_pixel(hexagon);

    m_hexagon.setPosition(x, y);
    m_hexagon.setFillColor(colour);
    m_hexagon.setOutlineColor(sf::Color::Black);
    m_hexagon.setOutlineThickness(2);

    m_texture.setView(m_view);
    m_texture.draw(m_hexagon);
}

void Board::display(sf::RenderWindow &window)
{
    m_texture.display();

    sf::Sprite sprite;
    sprite.setTexture(m_texture.getTexture());
    window.setView(m_view);
    window.draw(sprite);
}
