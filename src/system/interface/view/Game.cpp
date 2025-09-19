#include "system/interface/view/Game.h"

#include "system/interface/widget/Board.h"
#include "system/logic/Runes.h"
#include "utility/Time.h"

GameInterface::GameInterface(
    MessagingSystem *event_system,
    RenderSystem *render_system,
    EntitySystem *ecs
  ) : Interface(event_system, render_system, ecs)
    , m_board(
        m_runes,
        Vector2i(render_system->screen_width(), render_system->screen_height()),
        Vector2d(20, 20)
    )
{
    m_scene.add_entity(m_board.entity());
    render_system->scene_set(m_scene);

    event_system->messenger().subscribe<CLICK>(
        [this](const Event<CLICK> &m) { handle_click(m); }
    );

    event_system->messenger().subscribe<MOUSE>(
        [this](const Event<MOUSE> &m) { handle_mouse(m); }
    );
}

std::unique_ptr<Interface> GameInterface::main(StopCondition &&stop)
{
    stop.wait();
    return {};
}

void GameInterface::handle_click(const Event<CLICK> &click)
{
    std::unique_lock lock(m_mutex);

    Hexagon::Hexagon<int> hex = m_board.grid().to_hexagon(click.x, click.y);

    if (click.button == sf::Mouse::Button::Left) {
        m_runes.perform<Runes::ActionType::PLACE_PLAYER_RUNE>(
            0, Runes::RuneType::VITALITY, hex
        );
    }
    else {
        m_runes.perform<Runes::ActionType::MOVE_PLAYER_RUNE>(0, hex, hex);
    }
}

void GameInterface::handle_mouse(const Event<MOUSE> &mouse)
{
    static Hexagon::Hexagon<int> last;
    std::scoped_lock<std::mutex> lock(m_mutex);

    Hexagon::Hexagon<int> current = m_board.grid().to_hexagon(mouse.x, mouse.y);
    m_board.remove_highlight(last);
    m_board.add_highlight(current, sf::Color(50, 50, 50, 100));
    last = current;
}
