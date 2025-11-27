#pragma once

#include "system/logic/Runes.h"
#include "system/System.h"

class RuneSystem : public System
{
public:

    RuneSystem(ECS *ecs, EventManager *events, std::stop_token stop);

private:

    /// The game model.
    Runes m_runes;

    Subscriptions m_subscriptions;
};
