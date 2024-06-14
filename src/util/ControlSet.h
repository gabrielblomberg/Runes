#pragma once

#include <array>
#include <functional>
#include <vector>

#include <SFML/Window/Event.hpp>

/**
 * @brief A generic set of controls with some callbacks.
 */
class ControlSet
{
public:

    ControlSet()
        : m_enabled(true)
    {}

    using Callback = std::function<void(const sf::Event&)>;

    /**
     * @brief Enable all control set callbacks.
     */
    inline void enable() {
        m_enabled = true;
    }

    /**
     * @brief Disable all control set callbacks.
     */
    inline void disable() {
        m_enabled = false;
    }

    /**
     * @brief Add a callback to this control set.
     * 
     * @param event The event to callback on.
     * @param callback The function to call on the event.
     */
    inline void add_control(sf::Event::EventType event, Callback callback) {
        m_callbacks[event].push_back(callback);
    }

    /**
     * @brief Handle an event using this control set.
     * 
     * @param event 
     */
    inline void handle(const sf::Event &event) {
        if (m_enabled)
            for (auto &callback : m_callbacks[event.type])
                callback(event);
    }

private:

    /// All the callbacks for this control set.
    std::array<std::vector<Callback>, sf::Event::EventType::Count> m_callbacks;

    /// If the control set is enabled or disabled.
    bool m_enabled;
};
