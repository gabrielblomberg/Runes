#include "system/render/RenderSystem.h"

#include "system/EntitySystem.h"
#include "utility/StopCondition.h"

RenderSystem::RenderSystem(
    ECS *ecs,
    EventManager *events,
    sf::RenderWindow *window,
    std::stop_token stop
  ) : System(ecs, events, stop)
    , m_window(window)
{}

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
