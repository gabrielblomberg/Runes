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
    InterfaceSystem(
        RenderSystem *render_system,
        MessagingSystem *event_system,
        std::stop_token stop
    );

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
    RenderSystem *m_render_system;
    MessagingSystem *m_event_system;
    std::jthread m_thread;
};

