#pragma once

#include <thread>

#include "Message.h"
#include "util/Messenger.h"
#include "interface/Window.h"
#include "util/StopCondition.h"

class Application;

/**
 * @brief An application state, such as main menu or game screen.
 */
class State
{
public:

    /**
     * @brief Create a new application state.
     * @param application Pointer to the main application instance.
     */
    State(Application *application, std::stop_token stop)
        : m_handle(application)
        , m_stop(stop)
    {}

    /**
     * @brief Main method to run the state.
     * 
     * @return The next state, or nullptr on application exit.
     */
    virtual std::unique_ptr<State> main() = 0;

protected:

    /// Pointer to the main application instance.
    Application *const m_handle;

    /// Condition to stop on.
    StopCondition m_stop;
};

class Application
{
public:

    /**
     * @brief Initialise the application.
     */
    Application();

    /**
     * @brief The main thread that handles input events.
     * @param stop Stop signal to exit.
     */
    void main();

    /**
     * @brief The thread controlling the logic of the application.
     * @param stop Stop signal to exit.
     */
    void state_machine(std::stop_token stop);

    /**
     * @brief Get a reference to the messenger.
     */
    inline Messenger<Topics> &messenger() {
        return m_messenger;
    }

    /**
     * @brief Get a reference to the window.
     */
    inline Window &window() {
        return m_window;
    }

private:

    /// The window that states can draw to.
    Window m_window;

    /// The messenger for messages between different parts of the program.
    Messenger<Topics> m_messenger;

    /// State of the application.
    std::unique_ptr<State> m_state;

    /// Thread running the application.
    std::jthread m_state_thread;

    /// Stop source for stopping the application.
    std::stop_source m_stop;
};
