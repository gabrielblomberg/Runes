#include "interface/Rune.h"

#include "util/Hexagon.h"
#include "model/Runes.h"

Entity rune_create(ECS &ecs, Type type)
{
    Entity entity = ecs.create_entity();
    ecs.add_component<ComponentType::Position>(entity, {0.0, 0.0});
    ecs.add_component<ComponentType::Renderable>(entity, [this])
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
