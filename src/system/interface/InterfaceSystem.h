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
        std::stop_source stop
    );

    /**
     * @brief Start the interface.
     */
    void start();

    void handle_input();

private:

    friend class Interface;

    void main();

    std::stop_source m_stop;
    std::unique_ptr<Interface> m_interface;
    Messaging *m_messenger;
    std::jthread m_thread;
};

