#pragma once

#include <stop_token>

#include "ECS.h"
#include "ECSEvents.h"
#include "system/interface/InterfaceSystem.h"
#include "system/logic/RuneSystem.h"
#include "system/render/RenderSystem.h"
#include "utility/SynchronousEventQueue.h"

class Application
{
public:

    Application();

    void main();

    void step_input();

private:

    std::unique_ptr<sf::RenderWindow> create_window();

    std::stop_source m_stop_source;
    std::unique_ptr<sf::RenderWindow> m_window;

    ECS m_ecs;
    ECSEvents m_ecs_events;

    RuneSystem m_rune_system;
    InterfaceSystem m_interface_system;
    RenderSystem m_render_system;
};
