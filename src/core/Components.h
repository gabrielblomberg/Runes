#pragma once

#include <functional>

enum class ComponentType {
    Position,
    Renderable
};

template<ComponentType C>
struct Component {};

template<>
struct Component<ComponentType::Position> {
    double x;
    double y;
};

template<>
struct Component<ComponentType::Renderable> {
    std::function<void()> render;
};
