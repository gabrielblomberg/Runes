#include "system/render/RenderSystem.h"

#include "system/EntitySystem.h"
#include "utility/StopCondition.h"

RenderSystem::RenderSystem(ECS *ecs, Messaging *messenger, std::stop_token stop)
    : System(ecs, messenger, stop)
{
    // Determine the largest fullscreen mode.
    auto modes = sf::VideoMode::getFullscreenModes();
    auto mode = std::max_element(
        modes.begin(),
        modes.end(),
        [](sf::VideoMode &a, sf::VideoMode &b) {
            return (a.size.x * a.size.y) < (b.size.x * b.size.y);
        }
    );

    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(mode->size),
        "Runes",
        sf::Style::Default
    );

    m_window->setActive(false);
}

RenderSystem::~RenderSystem()
{
    m_window->close();
}

void RenderSystem::step(double t)
{
    m_window->setActive(true);
    m_window->clear();

    auto &window = *m_window;
    for (Entity entity : m_ecs->get_cache(1 << Renderable)) {
        if (!m_stop) {
            ecs.get_component<Renderable>(entity).render(window, t);
        }
    }

    m_window->display();
}
