#include "object/Rune.h"

#include "utility/Hexagon.h"
#include "Runes.h"
#include "system/EntitySystem.h"

Entity rune_create(Type type)
{
    Entity entity = s_ecs.create_entity();
    s_ecs.add_component<Position>(entity, {0.0, 0.0});
    s_ecs.add_component<Renderable>(
        entity,
        [this](RenderLock &renderer){
            static 
        }
    );
    return entity;
}

void rune_click()
{

}

void rune_render(RenderSystem &renderer)
{
    // Create a hexagon that will be drawn.
    const static sf::ConvexShape shape = []{
        sf::ConvexShape shape;
        shape.setPointCount(6);
        for (int i = 0; i < 6; i++) {
            auto [x, y] = m_grid.corner_offset(i);
            m_hexagon.setPoint(i, Vector2d(x, y) * 0.95);
        }
        return shape;
    }();
    
    renderer.
}
