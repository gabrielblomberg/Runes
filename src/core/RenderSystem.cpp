#include "core/RenderSystem.h"

#include "util/StopCondition.h"

void Scene::add_entity(Entity entity)
{
    m_entities.push_back(entity);
}

void Scene::remove_entity(Entity entity)
{
    std::unique_lock lock(m_mutex);
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it != m_entities.end())
        m_entities.erase(it);
}

Scene::~Scene()
{
    std::unique_lock lock(m_mutex);
    for (Entity entity : m_entities)
        m_render_system->m_ecs->remove_entity(entity);
}

RenderSystem::RenderSystem(ECS &ecs, std::string &&title)
    : m_window()
    , m_ecs(&ecs)
    , m_current_scene(nullptr)
{
    // Determine the largest fullscreen mode.
    auto modes = sf::VideoMode::getFullscreenModes();
    auto mode = std::max_element(
        modes.begin(),
        modes.end(),
        [](sf::VideoMode &a, sf::VideoMode &b) {
            return (a.width * a.height) < (b.width * b.height);
        }
    );

    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(mode->width, mode->height),
        title.c_str(),
        sf::Style::Default
    );

    m_window->setActive(false);
}

RenderSystem::~RenderSystem()
{
    RenderLock window_lock(m_window.get(), &m_mutex);
    m_window->close();
}

void RenderSystem::start(std::stop_token stop)
{
    if (m_render_thread.get_id() == std::jthread::id())
        m_render_thread = std::jthread(&RenderSystem::render_thread, this, stop);
}

void RenderSystem::scene_set(Scene &scene)
{
    std::unique_lock lock(m_mutex);
    m_current_scene = &scene;
}

void RenderSystem::render_thread(std::stop_token stop)
{
    // ~144Hz
    const static Time::Duration delta = 7ms;
    StopCondition stop_condition {stop};

    while (!stop_condition) {

        {
            auto render_lock = lock();
            m_window->clear();

            if (m_current_scene) {
                std::unique_lock lock(m_current_scene->m_mutex);
                for (Entity entity : m_current_scene->m_entities)
                    m_ecs->get_component<Renderable>(entity).render(render_lock);
                m_window->display();
            }
        }

        stop_condition.wait_until(Time::now() + delta);
    }
}
