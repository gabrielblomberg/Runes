#include "interface/Button.h"

#include <iostream>

Button::Button(int px, int py, int sx, int sy, std::function<void(bool)> callback)
    : m_px(px)
    , m_py(py)
    , m_sx(sx)
    , m_sy(sy)
    , m_callback(callback)
{}

void Button::click(int px, int py, bool pressed)
{
    if (m_px < px && px < m_px + m_sx) {
        if (m_py < py && py < m_py + m_sy) {
            m_callback(pressed);
        }
    }
}

void Button::display(Window &window)
{
    sf::Vertex vertices[] = {
        sf::Vertex(sf::Vector2f(m_px, m_py), sf::Color::Red),
        sf::Vertex(sf::Vector2f(m_px + m_sx, m_py), sf::Color::Red),
        sf::Vertex(sf::Vector2f(m_px + m_sx, m_py + m_sy), sf::Color::Red),
        sf::Vertex(sf::Vector2f(m_px, m_py + m_sy), sf::Color::Red)
    };

    sf::View view;
    view.setCenter(m_px, m_py);
    window->setView(view);
    window->draw(vertices, 4, sf::Quads);
}
