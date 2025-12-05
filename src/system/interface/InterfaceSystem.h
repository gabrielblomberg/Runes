#pragma once

#include <stop_token>
#include <thread>
#include <vector>

#include "ECS.h"
#include "utility/StopCondition.h"

class InterfaceSystem;

class Interface
{
public:

    inline Interface(InterfaceSystem* interface_system);

    inline virtual ~Interface() {};

    virtual std::unique_ptr<Interface> step() = 0;

protected:

    std::stop_token m_stop;
    InterfaceSystem* m_interface_system;
    ECS* m_ecs;
};

class InterfaceSystem
{
public:

    /**
     * @brief Create the interface.
     */
    InterfaceSystem(ECS* ecs, std::stop_token stop);

    void step();

private:

    friend class Interface;

    void main();

    std::stop_token m_stop;
    ECS* m_ecs;
    std::unique_ptr<Interface> m_state;
};
