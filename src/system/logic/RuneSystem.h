#pragma once

#include "system/logic/Runes.h"

class RuneSystem
{
public:

    RuneSystem() = default;

    Runes &get() {
        return m_runes;
    }

private:

    /// The game model.
    Runes m_runes;
};
