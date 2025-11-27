#include "system/object/Board.h"

Board::Board(Vector2i pixel_dimensions, Vector2d hexagon_size)
    : m_entity(s_ecs.create_entity())
    , m_texture()
    , m_grid()
    , m_view()
    , m_hexagon()
{
    // Options for the texture storing the hexagonal grid.
    sf::ContextSettings texture_settings;
    texture_settings.antiAliasingLevel = 8;

    // Create the texture that contains the board.
    if (!m_texture.resize(pixel_dimensions, texture_settings))
        throw std::runtime_error("Failed to create runes texture.");

    // Define the transformation from the texture to the window. The texture has
    // the given size and should be drawn in the middle of the screen.
    m_view.setSize(pixel_dimensions);
    m_view.setCenter(pixel_dimensions / 2);

    // Define the hexagonal grid to have the same size d
    m_grid = Hexagon::Grid<Hexagon::GridType::FLAT>(
        hexagon_size.x,
        hexagon_size.y,
        pixel_dimensions.x / 2,
        pixel_dimensions.y / 2
    );

    // Create a hexagon that will be drawn.
    m_hexagon.setPointCount(6);
    for (int i = 0; i < 6; i++) {
        auto [x, y] = m_grid.corner_offset(i);
        m_hexagon.setPoint(i, Vector2d(x, y) * 0.95);
    }

    ecs.add_component<Renderable>(
        m_entity,
        [this](sf::RenderWindow &window){ render(window); }
    );
}

void Board::render(sf::RenderWindow &window)
{
    m_texture.clear();

    // sf::Color colour = m_runes->connected() ? sf::Color::White : sf::Color::Red;

    m_hexagon.setFillColor(sf::Color::Black);
    m_hexagon.setOutlineThickness(1);
    m_texture.setView(m_view);

    for (const auto &[a, vertex] : m_runes->board().vertices()) {
        render_hexagon(a, colour);
        auto [x0, y0] = m_grid.to_pixel(a);

        for (const auto &[b, edge] : vertex->edges) {
            auto [x1, y1] = m_grid.to_pixel(b);

            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(x0, y0)),
                sf::Vertex(sf::Vector2f(x1, y1))
            };

            m_texture.draw(line, 2, sf::PrimitiveType::Lines);
        }
    }

    for (auto &[hex, colour] : m_highlights)
        render_hexagon(hex, colour);

    m_texture.display();

    sf::Sprite sprite {m_texture.getTexture()};

    window->setView(m_view);
    window->draw(sprite);
}

void Board::render_hexagon(Hexagon::Hexagon<int> hexagon, sf::Color colour)
{
    auto [x, y] = m_grid.to_pixel(hexagon);

    m_hexagon.setPosition(sf::Vector2f(x, y));
    m_hexagon.setFillColor(colour);
    m_hexagon.setOutlineColor(sf::Color::Black);
    m_hexagon.setOutlineThickness(2);

    m_texture.setView(m_view);
    m_texture.draw(m_hexagon);
}
