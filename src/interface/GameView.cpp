#include "interface/GameView.h"

GameView::GameView(Runes &runes, Window &window)
    : m_runes(runes)
    , m_board(Vector2i(window->getSize().x, window->getSize().y), Vector2i(0, 0), Vector2d(100, 100))
{
    auto lock = window.lock();
    m_board.draw(window, runes);
}

void GameView::display(Window &window)
{
    m_board.display(window);
}

void GameView::handle_click(int x, int y)
{
    // Determine the hexagon that was clicked on.
    Hexagon::Hexagon<int> hexagon = m_board.grid().to_hexagon(x, y);
    std::cout << "Click hexagon " << hexagon << std::endl;
    m_board.draw_hexagon(hexagon);
}
