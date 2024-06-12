#pragma once

#include <SFML/Graphics.hpp>
#include "util/Vector2.h"

class Renderable
{
    /**
     * @brief Draw to a target.
     * 
     * The view of the target should be set prior to calling this function.
     * 
     * @param target The target to draw the object to, a texture or the main
     * window. 
     */
    virtual void draw(sf::RenderTarget &target);
};

/**
 * @brief A generic object that exists in the world space to some capacity.
 */
class Object
{
public:

    /**
     * @brief Create a new object at a position in the world.
     * 
     */
   Object(Vector2d world_position);

};
