#pragma once

#include <mutex>

#include "system/EntitySystem.h"

/**
 * @brief A scene.
 */
class Scene
{
public:

    Scene() = default;

    /// Cannot copy a scene.
    inline Scene(const Scene&) = delete;
    inline Scene operator=(const Scene&) = delete;

    /**
     * @brief Move a scene.
     */
    inline Scene(Scene &&scene)
    {
        std::unique_lock lock(scene.m_mutex);
        m_entities = std::move(scene.m_entities);
    }

    /**
     * @brief Move a scene.
     */
    inline Scene &operator=(Scene &&scene)
    {
        std::unique_lock lock(scene.m_mutex);
        m_entities = std::move(scene.m_entities);
        return *this;
    }

    /**
     * @brief Add an entity to a scene.
     */
    inline void add_entity(Entity entity)
    {
        m_entities.push_back(entity);
    }

    /**
     * @brief Remove an entity from the scene.
     */
    inline void remove_entity(Entity entity)
    {
        std::unique_lock lock(m_mutex);
        auto it = std::find(m_entities.begin(), m_entities.end(), entity);
        if (it != m_entities.end())
            m_entities.erase(it);
    }

private:

    /// The entities in the scene.
    std::vector<Entity> m_entities;

    /// Mutex for concurrent access to the scene.
    std::mutex m_mutex;
};
