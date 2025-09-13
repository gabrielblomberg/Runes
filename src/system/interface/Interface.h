#pragma once

#include "system/MessagingSystem.h"
#include "system/render/RenderSystem.h"
#include "system/EntitySystem.h"

#include "utility/StopCondition.h"

/**
 * @brief An game state, such as main menu or game screen.
 */
class Interface
{
public:

    /**
     * @brief Construct the game state.
     * 
     * @param interface Pointer to the interface.
     */
    inline Interface(
        MessagingSystem *event_system,
        RenderSystem *render_system,
        EntitySystem *entity_system
      ) : m_event_system(event_system)
        , m_render_system(render_system)
        , m_entity_system(entity_system)
    {}

    /**
     * @brief Virtual destructor for inheritance.
     */
    inline virtual ~Interface() {};

    /**
     * @brief Main method to run the state.
     * 
     * @param stop The stop token to stop on.
     * @return The next state, or nullptr if none.
     */
    virtual std::unique_ptr<Interface> main(StopCondition &&stop) = 0;

protected:

    MessagingSystem *m_event_system;

    RenderSystem *m_render_system;

    EntitySystem *m_entity_system;
};
