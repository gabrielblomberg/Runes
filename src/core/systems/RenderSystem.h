#pragma once

#include "util/EntityComponentSystem.h"

class RenderSystem
{

private:

    template<typename Systems, typename Components, std::size_t N>
    friend class EntityComponentSystem<Systems, Components, N>;

};
