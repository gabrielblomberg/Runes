#include "state/GameState.h"

#include "object/Board.h"
#include "Runes.h"
#include "utility/Time.h"

GameState::GameState(Game &game)
    : Game::State(&game)
    , m_scene(game.renderer().scene_create())
    , m_board(
        game.ecs(),
        m_runes,
        Vector2i(game.renderer().screen_width(), game.renderer().screen_height()),
        Vector2d(20, 20)
    )
{
    m_scene.add_entity(m_board.entity());
    game.renderer().scene_set(m_scene);

    game.messenger().subscribe<CLICK>(
        [this](const Event<CLICK> &m) { handle_click(m); }
    );

    game.messenger().subscribe<MOUSE>(
        [this](const Event<MOUSE> &m) { handle_mouse(m); }
    );
}

std::vector<std::unique_ptr<Game::State>> GameState::run(StopCondition &&stop)
{
    stop.wait();
    return {};
}

void GameState::handle_click(const Event<CLICK> &click)
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

void GameState::handle_mouse(const Event<MOUSE> &mouse)
{
    static Hexagon::Hexagon<int> last;
    std::scoped_lock<std::mutex> lock(m_mutex);

    Hexagon::Hexagon<int> current = m_board.grid().to_hexagon(mouse.x, mouse.y);
    m_board.remove_highlight(last);
    m_board.add_highlight(current, sf::Color(50, 50, 50, 100));
    last = current;
}
