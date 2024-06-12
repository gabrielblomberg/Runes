#include "states/GameState.h"

#include "interface/Window.h"
#include "interface/Board.h"
#include "interface/Button.h"
#include "model/Runes.h"
#include "util/Vector2.h"
#include "util/Time.h"

GameState::GameState(Application *app, std::stop_token stop)
    : ApplicationState(app, stop)
    , m_board(
        Vector2i(app->window()->getSize().x, app->window()->getSize().y),
        Vector2d(20, 20)
    )
    , m_box(
        Vector2i(10 * 20, 6 * 20),
        20,
        Vector2i(app->window()->getSize().x / 2, 4 * app->window()->getSize().y / 5)
    )
{
    auto size = app->window()->getSize();
    m_screen_pixels = Vector2i(size.x, size.y);

    auto window = app->window().lock();
    window->clear(sf::Color::Black);
    window->display();

    m_handle->messenger().subscribe<CLICK>(
        [this](const Message<CLICK> &m) { handle_click(m); }
    );

    m_handle->messenger().subscribe<MOUSE>(
        [this](const Message<MOUSE> &m) { handle_mouse(m); }
    );

    m_render_thread = std::jthread(&GameState::render_thread, this);
}

void GameState::handle_click(const Message<CLICK> &click)
{
    std::scoped_lock<std::mutex> lock(m_mutex);

    Hexagon::Hexagon<int> hex = m_board.grid().to_hexagon(click.x, click.y);

    if (click.button == 0) {
        m_runes.perform<Runes::ActionType::PLACE_PLAYER_RUNE>(
            0, Runes::RuneType::VITALITY, hex
        );
        m_box.add(Runes::RuneType::VITALITY);
    }
    else {
        m_box.remove(Runes::RuneType::VITALITY);
        m_runes.perform<Runes::ActionType::MOVE_PLAYER_RUNE>(0, hex, hex);
    }
}

void GameState::handle_mouse(const Message<MOUSE> &mouse)
{
    static Hexagon::Hexagon<int> last;
    std::scoped_lock<std::mutex> lock(m_mutex);

    Hexagon::Hexagon<int> current = m_board.grid().to_hexagon(mouse.x, mouse.y);
    m_board.remove_highlight(last);
    m_board.add_highlight(current, sf::Color(50, 50, 50, 100));
    last = current;
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

            m_box.draw(*window);
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
