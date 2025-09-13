#include "system/MessagingSystem.h"
#include "system/interface/InterfaceSystem.h"
#include "system/render/RenderSystem.h"
#include "system/logic/RuneSystem.h"
#include "system/EntitySystem.h"

int main(int argc, char **argv)
{
    std::stop_source stop_source;
    std::stop_token stop_token = stop_source.get_token();

    EntitySystem entity_system;

    RenderSystem render_system(&entity_system, stop_token);
    MessagingSystem event_system(render_system.lock().get(), std::move(stop_source));
    InterfaceSystem interface_system(&render_system, &event_system, stop_token);

    render_system.start();
    interface_system.start();
    event_system.main();

    return 0;
}
