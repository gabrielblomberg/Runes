#pragma once

#include <stop_token>

#include "ECS.h"
#include "Messaging.h"

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
     * @param messenger The messenger.
     * @param stop The stop signal for the program.
     */
    System(ECS *ecs, Messaging *messenger, std::stop_token stop)
        : m_stop(stop)
        , m_ecs(ecs)
        , m_messenger(messenger)
    {}

private:

    /// Stop signal for the program.
    std::stop_token m_stop;

    /// Entity component system.
    ECS *m_ecs;

    /// Messenger.
    Messaging *m_messenger;
};
