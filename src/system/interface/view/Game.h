#pragma once

#include <functional>

#include "system/interface/Interface.h"
#include "system/logic/RuneSystem.h"

#include "system/interface/widget/Board.h"

class GameInterface : public Interface
{
public:

    /**
     * @brief Instantiate a new game state.
     */
    GameInterface(
        MessagingSystem *event_system,
        RenderSystem *render_system,
        EntitySystem *ecs
    );

    /**
     * @brief Runs the main game state.
     * @return Nullptr indicating game exit.
     */
    virtual std::unique_ptr<Interface> main(StopCondition &&stop) override;

    /**
     * @brief Allows threads to join before destroying stop condition.
     */
    virtual ~GameInterface() {}; 

private:

    /**
     * @brief Handle a click.
     */
    void handle_click(const Event<CLICK> &click);

    /**
     * @brief Handle mouse movement.
     */
    void handle_mouse(const Event<MOUSE> &mouse);

    /// Mutex protecting concurrent access to the game state.
    std::mutex m_mutex;

    Board m_board;
};
