#include "system/interface/InterfaceSystem.h"

#include "system/interface/view/Game.h"
#include "system/render/RenderSystem.h"
#include "system/MessagingSystem.h"

InterfaceSystem::InterfaceSystem(
    RenderSystem *render_system,
    MessagingSystem *event_system,
    std::stop_token stop
  ) : m_stop(stop)
    , m_render_system(render_system)
    , m_event_system(event_system)
{}

void InterfaceSystem::main()
{
    m_interface = std::make_unique<GameInterface>(*this);
    while (m_interface && !m_stop) {
        m_interface = state->run(StopCondition(m_stop));
    }
}

void InterfaceSystem::start()
{
    if (!m_thread) {
        m_thread = std::jthread(&InterfaceSystem::main, this)
    }
}
