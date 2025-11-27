#pragma once

#include <stop_token>
#include <vector>
#include <thread>

#include "utility/StopCondition.h"
#include "EventManager.h"
#include "ECS.h"

class InterfaceSystem;

class Interface
{
public:

    inline Interface(InterfaceSystem *interface_system);

    inline virtual ~Interface() {};

    virtual std::unique_ptr<Interface> step() = 0;

protected:

    std::stop_token m_stop;
    InterfaceSystem *m_interface_system;
    ECS *m_ecs;
    EventManager *m_events;
};

class InterfaceSystem
{
public:

    /**
     * @brief Create the interface.
     */
    InterfaceSystem(ECS *ecs, EventManager *events, std::stop_token stop);

    void step();

private:

    friend class Interface;

    void main();

    std::stop_token m_stop;
    ECS *m_ecs;
    EventManager *m_events;
    std::unique_ptr<Interface> m_state;
};
