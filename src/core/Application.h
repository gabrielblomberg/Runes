#pragma once

#include <thread>

#include "core/Message.h"
#include "util/Messenger.h"
#include "util/StopCondition.h"
#include "interface/Window.h"

class Application
{
public:

    /**
     * @brief An application state, such as main menu or game screen.
     */
    class State
    {
    public:

        /**
         * @brief Construct the application state.
         * 
         * @param application Pointer to the owning application.
         */
        inline State(Application *application)
            : m_application(application)
        {}

        /**
         * @brief Virtual destructor for inheritance.
         */
        inline virtual ~State() {};

        /**
         * @brief Main method to run the state.
         * 
         * @param stop The stop token to stop on.
         * 
         * @return The next state, or nullptr if none.
         */
        virtual std::vector<std::unique_ptr<State>> run(StopCondition &&stop) = 0;

    protected:

        // Pointer to the owning application.
        Application *m_application;
    };

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
    void run(std::stop_token stop);

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

    /// Stop source for stopping the application.
    std::stop_source m_stop;

    /// The window that states can draw to.
    Window m_window;

    /// The messenger for messages between different parts of the program.
    Messenger<Topics> m_messenger;

    /// State of the application.
    std::vector<std::unique_ptr<State>> m_states;

    /// Thread running the application.
    std::jthread m_state_thread;
};
