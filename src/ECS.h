#pragma once

#include "utility/EntityComponentSystem.h"
#include "system/logic/Runes.h"

#include <functional>

inline const constexpr std::int64_t MAX_ENTITIES = 1024;

enum ComponentType : std::size_t {
    Renderable,
    Position,
    Rune,
    Player
};

template<ComponentType C>
struct Component {};

/**
 * @brief Component position.
 */
template<>
struct Component<ComponentType::Position> {
    double x;
    double y;
};

class RenderLock;

/**
 * @brief Function callback for the renderer.
 */
using RenderFunction = std::function<void(RenderLock&)>;

/**
 * @brief Component render function.
 */
template<>
struct Component<ComponentType::Renderable> {
    RenderFunction render;
};

/**
 * @brief Data for a rune.
 */
template<>
struct Component<ComponentType::Rune> {
    Runes::RuneType type;
    std::size_t player_id;
};

/**
 * @brief Data for a player.
 */
template<>
struct Component<ComponentType::Player> {
    std::size_t id;
    std::string name;
    std::unordered_map<Runes::RuneType, std::size_t> inventory;
};

/**
 * @brief Components.
 */
using Components = TypeList::TypeList<
    Component<ComponentType::Renderable>,
    Component<ComponentType::Position>,
    Component<ComponentType::Rune>,
    Component<ComponentType::Player>    
>;

/**
 * @brief The entity component system used for the game.
 */
using ECS = EntityComponentSystem<Components, MAX_ENTITIES>;
