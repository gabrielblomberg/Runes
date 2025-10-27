#pragma once

#include <stop_token>
#include <vector>
#include <thread>

#include "utility/StopCondition.h"
#include "system/interface/Interface.h"

class InterfaceSystem
{
public:

    /**
     * @brief Create the interface.
     */
    InterfaceSystem(ECS *ecs, EventManager *events, std::stop_token stop);

    /**
     * @brief Start the interface.
     */
    void start();

    void handle_input();

private:

    friend class Interface;

    void main();

    std::stop_token m_stop;
    std::unique_ptr<Interface> m_interface;
    std::jthread m_thread;
};
