#pragma once

#include "core/Application.h"
#include "model/Runes.h"
#include "util/Hexagon.h"
#include "util/Vector2.h"

namespace interface {

class Rune
{
public:

    using Type = Runes::RuneType;

    static Rune Create(Type type);

    void handle_click();

    void draw();

private:

    Entity m_entity;

    Rune();
};

} // namespace interface
