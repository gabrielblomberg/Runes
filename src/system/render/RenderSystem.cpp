#include "system/render/RenderSystem.h"

#include "system/EntitySystem.h"
#include "utility/StopCondition.h"

RenderSystem::RenderSystem(EntitySystem *entity_system, std::stop_token stop)
    : m_stop(stop)
    , m_entity_system(entity_system)
    , m_current_scene(nullptr)
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
    RenderLock window_lock(m_window.get(), &m_mutex);
    m_window->close();
}

void RenderSystem::start()
{
    // If not already started.
    if (m_thread.get_id() == std::jthread::id())
        m_thread = std::jthread(&RenderSystem::main, this);
}

void RenderSystem::scene_set(Scene &scene)
{
    std::unique_lock lock(m_mutex);
    m_current_scene = &scene;
}

void RenderSystem::main()
{
    auto &entity_system = *m_entity_system;

    // ~144Hz
    const static Time::Duration delta = 7ms;
    StopCondition stop_condition {m_stop};

    while (!stop_condition) {
        auto wait_until = Time::now() + delta;

        {
            auto render_lock = lock();
            m_window->clear();

            if (m_current_scene) {
                std::unique_lock lock(m_current_scene->m_mutex);
                for (Entity entity : m_current_scene->m_entities)
                    entity_system.get_component<Renderable>(entity).render(render_lock);
                m_window->display();
            }
        }

        stop_condition.wait_until(wait_until);
    }
}
