#include "system/interface/view/Game.h"

#include "system/interface/widget/Board.h"
#include "system/logic/Runes.h"
#include "utility/Time.h"

GameInterface::GameInterface(InterfaceSystem *interface_system)
    : Interface(interface_system)
    , m_click(m_events->click.subscribe_sync())
    , m_mouse(m_events->mouse.subscribe_sync())
    , m_board(
        Vector2i(m_events->screen.get().width, m_events->screen.get().height),
        Vector2d(20, 20)
    )
{
}

std::unique_ptr<Interface> GameInterface::step()
{
    while (auto&& click = m_click.get())
        handle_click(*click);

    while (auto&& mouse = m_mouse.get())
        handle_mouse(*mouse);

    return nullptr;
}

void GameInterface::handle_click(const Message::Click &click)
{
    std::unique_lock lock(m_mutex);

    Hexagon::Hexagon<int> hex = m_board.grid().to_hexagon(click.x, click.y);

    if (click.button == sf::Mouse::Button::Left) {
        m_events.place_rune.request()
        m_runes.perform<Runes::ActionType::PLACE_PLAYER_RUNE>(
            0, Runes::RuneType::VITALITY, hex
        );
    }
    else {
        m_runes.perform<Runes::ActionType::MOVE_PLAYER_RUNE>(0, hex, hex);
    }
}

void GameInterface::handle_mouse(const Message::Mouse &mouse)
{
    static Hexagon::Hexagon<int> last;
    std::scoped_lock<std::mutex> lock(m_mutex);

    Hexagon::Hexagon<int> current = m_board.grid().to_hexagon(mouse.x, mouse.y);
    m_board.remove_highlight(last);
    m_board.add_highlight(current, sf::Color(50, 50, 50, 100));
    last = current;
}
