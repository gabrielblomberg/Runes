#include "core/GameState.h"

#include "interface/Board.h"
#include "interface/Button.h"
#include "interface/Window.h"
#include "model/Runes.h"
#include "util/Time.h"

GameState::GameState(Application *app)
    : Application::State(app)
    , m_board(
        Vector2i(app->window()->getSize().x, app->window()->getSize().y),
        Vector2d(20, 20)
    )
{
    auto size = app->window()->getSize();
    m_screen_pixels = Vector2i(size.x, size.y);

    auto window = app->window().lock();
    window->clear(sf::Color::Black);
    window->display();

    m_application->messenger().subscribe<CLICK>(
        [this](const Message<CLICK> &m) { handle_click(m); }
    );

    m_application->messenger().subscribe<MOUSE>(
        [this](const Message<MOUSE> &m) { handle_mouse(m); }
    );
}

std::vector<std::unique_ptr<Application::State>> GameState::run(StopCondition &&stop)
{
    m_render_thread = std::jthread(&GameState::render_thread, this, stop);
    stop.wait();
    return {};
}

void GameState::handle_click(const Message<CLICK> &click)
{
    std::scoped_lock<std::mutex> lock(m_mutex);

    Hexagon::Hexagon<int> hex = m_board.grid().to_hexagon(click.x, click.y);

    if (click.button == 0) {
        m_runes.perform<Runes::ActionType::PLACE_PLAYER_RUNE>(
            0, Runes::RuneType::VITALITY, hex
        );
    }
    else {
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

void GameState::render_thread(StopCondition &&stop)
{
    // ~144Hz
    static Time::Duration delta = 7ms;

    while (!stop) {

        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            auto window = m_application->window().lock();
            window->clear();

            m_board.draw(m_runes);
            m_board.display(*window);
            window->display();
        }

        stop.wait_until(Time::now() + delta);
    }
}
