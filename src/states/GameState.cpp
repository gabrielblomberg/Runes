#include "states/GameState.h"

#include "interface/Window.h"
#include "interface/Board.h"
#include "interface/Button.h"
#include "model/Runes.h"
#include "util/Vector2.h"
#include "util/Time.h"

GameState::GameState(Application *app, std::stop_token stop)
    : ApplicationState(app, stop)
    , m_runes()
    , m_board(
        Vector2i(app->window()->getSize().x, app->window()->getSize().y),
        Vector2d(20, 20)
    )
{
    auto window = app->window().lock();
    window->clear(sf::Color::Black);
    window->display();

    m_handle->messenger().subscribe<CLICK>(
        [this](const Message<CLICK> &m) { handle_click(m); }
    );

    m_render_thread = std::jthread(&GameState::render_thread, this);
}

void GameState::handle_click(const Message<CLICK> &click)
{
    std::scoped_lock<std::mutex> lock(m_mutex);

    Hexagon::Hexagon<int> hex = m_board.grid().to_hexagon(click.x, click.y);

    if (click.button == 0)
        m_runes.perform<Runes::ActionType::PLACE_PLAYER_RUNE>(
            0, Runes::RuneType::VITALITY, hex
        );
    else
        m_runes.perform<Runes::ActionType::MOVE_PLAYER_RUNE>(0, hex, hex);
}

void GameState::render_thread()
{
    // ~144Hz
    static Time::Duration delta = 7ms;

    while (!m_stop) {

        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            auto window = m_handle->window().lock();
            window->clear();
            m_board.draw(m_runes);
            m_board.display(*window);
            window->display();
        }

        m_stop.wait_until(Time::now() + delta);
    }
}

std::unique_ptr<ApplicationState> GameState::main()
{
    m_stop.wait();
    return nullptr;
}
