#include "states/GameState.h"

GameState::GameState(Application *app, std::stop_token stop)
    : State(app, stop)
    , m_runes()
    , m_view(m_runes, app->window())
{
    m_handle->messenger().subscribe<CLICK>(
        [this](const Message<CLICK> &m) { handle_click(m); }
    );
}

void GameState::handle_click(Message<CLICK> click)
{
    auto &window = m_handle->window();
    auto lock = window.lock();

    m_view.handle_click(click.x, click.y);
    m_view.display(window);
    window->display();
}

std::unique_ptr<State> GameState::main()
{
    m_stop.wait();
    return nullptr;
}
