#pragma once

#include "ECS.h"

#include "system/interface/InterfaceSystem.h"
#include "system/render/RenderSystem.h"
#include "system/logic/RuneSystem.h"
#include "system/EventSystem.h"

class Application
{
public:

    Application();

    int main();

private:

    std::unique_ptr<sf::RenderWindow> create_window();

    std::stop_source m_stop_source;
    std::unique_ptr<sf::RenderWindow> m_window;

    ECS m_ecs;
    EventManger m_event_manager;

    RuneSystem m_rune_system;
    InterfaceSystem m_interface_system;
    RenderSystem m_render_system;
};
