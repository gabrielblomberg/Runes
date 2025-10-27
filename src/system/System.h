#pragma once

#include <stop_token>

#include "ECS.h"
#include "EventManager.h"

/**
 * @brief A generic system.
 */
class System
{
public:

    /**
     * @brief Construct a new system.
     * 
     * @param ecs The entity component system.
     * @param stop The stop signal for the program.
     */
    System(ECS *ecs, EventManager *events, std::stop_token stop)
        : m_stop(stop)
        , m_ecs(ecs)
        , m_events(events)
    {}

private:

    /// Stop signal for the program.
    std::stop_token m_stop;

    /// Entity component system.
    ECS *m_ecs;

    /// The event manager.
    EventManager *m_events;
};
