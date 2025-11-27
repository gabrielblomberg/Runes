#pragma once

#include <functional>

#include "system/interface/InterfaceSystem.h"
#include "system/logic/RuneSystem.h"

#include "system/interface/widget/Board.h"

class GameInterface : public Interface
{
public:

    /**
     * @brief Instantiate a new game state.
     */
    GameInterface(InterfaceSystem *interface_system);

    /**
     * @brief Runs the main game state.
     * @return Nullptr indicating game exit.
     */
    virtual std::unique_ptr<Interface> step();

    /**
     * @brief Allows threads to join before destroying stop condition.
     */
    virtual ~GameInterface() {}; 

private:

    /**
     * @brief Handle a click.
     */
    void handle_click(const Message::Click &click);

    /**
     * @brief Handle mouse movement.
     */
    void handle_mouse(const Message::Mouse &mouse);

    Topic::Click::SyncSubscription m_click;
    Topic::Mouse::SyncSubscription m_mouse;

    /// Mutex protecting concurrent access to the game state.
    std::mutex m_mutex;

    Board m_board;
};
