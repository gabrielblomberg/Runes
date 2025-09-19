#pragma once

#include "Game.h"
#include "system/RenderSystem.h"

Entity rune_create(EntitySystem &ecs, Type type);

void rune_click();

void rune_render(RenderSystem &renderer);
