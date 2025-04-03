#pragma once

#include "core/Game.h"
#include "core/RenderSystem.h"

Entity rune_create(ECS &ecs, Type type);

void rune_click();

void rune_render(RenderSystem &renderer);
