// Decompile-derived, 1.13c-only. Monster elemental enchant (cold variant); body shared with d2game_22320.cpp.
#include "d2game_enchant_22320.h"

// 1.13c D2Game+0x22490
// ECX = unit, EDX = modifier, stack unique (ret 4): cold min/max (stats 0x36/0x37), length 0x38 = level*5 + 100.
extern "C" void d2game_enchant_22490(uint32_t unit, uint32_t modifier, uint32_t unique, const D2GameEnchantEnv* env)
{
    d2game_enchant_body_22320(unit, modifier, unique, 0x36, false, env);
}
