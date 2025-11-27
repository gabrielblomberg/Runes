#include "system/interface/InterfaceSystem.h"

#include "system/interface/view/Game.h"
#include "system/render/RenderSystem.h"
#include "system/interface/InterfaceSystem.h"

Interface::Interface(InterfaceSystem *interface_system)
    : m_stop(interface_system->m_stop)
    , m_interface_system(interface_system)
    , m_ecs(interface_system->m_ecs)
    , m_events(interface_system->m_events)
{}

InterfaceSystem::InterfaceSystem(
    ECS *ecs,
    EventManager *events,
    std::stop_token stop
  ) : m_stop(stop)
    , m_ecs(ecs)
    , m_events(events)
    , m_state(std::make_unique<GameInterface>(this))
{}

void InterfaceSystem::step()
{
    if (!m_state)
        return;

    if (m_stop.stop_requested()) {
        m_state = nullptr;
        return;
    }

    m_state->step();
}
