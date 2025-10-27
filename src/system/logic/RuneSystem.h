#pragma once

#include "system/logic/Runes.h"
#include "system/System.h"

class RuneSystem : public System
{
public:

    RuneSystem(ECS *ecs, EventManager *events, std::stop_token stop)
        : System(ecs, events, stop)
    {}

    void step();

private:

    /// The game model.
    Runes m_runes;
};
