#pragma once

#include <functional>

#include "interface/Window.h"

class Button
{
public:

    Button() = default;

    /**
     * @brief Construct a new Button object
     * 
     * @param px The pixel x position.
     * @param py The pixel y position.
     * @param width The width of the button.
     * @param height The height of the button.
     * @param callback The function to call when the button is clicked. Takes
     * if the button was pressed or released.
     */
    Button(int px, int py, int sx, int sy, std::function<void(bool)> callback);

    /**
     * @brief Click the button.
     * 
     * @param px The x position of the click in pixels.
     * @param py The y position of the click in pixels.
     * @param pressed If the button is pressed after the click.
     */
    void click(int px, int py, bool pressed);

    /**
     * @brief Movement while the button is pressed.
     * 
     * @param px The x position of the mouse in pixels.
     * @param py The y position of the mouse in pixels.
     * @param pressed If the button is pressed.
     */
    void move(int px, int py, bool pressed);

    void display(Window &window);

private:
    int m_px, m_py, m_sx, m_sy;
    std::function<void(bool)> m_callback;
};
