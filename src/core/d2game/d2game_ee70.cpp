// src/core/d2game/d2game_ee70.cpp -- 1.13c D2Game+0x0EE70.
// EAX = game, ESI = unit; plain ret. Runs the 0xE7E0 callback over the game's unit table (0xDFC10, EDI = game,
// EBX = 0x6FC2E7E0, stack word = unit) and then tail-calls 0xE20A0 (ECX = game, EDX = unit); its result is returned.
#include <cstdint>
#include "d2game_seam.h"
// 1.13c D2Game+0x0EE70
extern "C" uint32_t D2Game_sub_EE70(const D2GameSeam& S, void* game, void* unit)
{
    const uint32_t g = (uint32_t)(uintptr_t)game, u = (uint32_t)(uintptr_t)unit;
    S(0xDFC10, g, 0, 0, 0x6FC2E7E0u, u, u);
    return S(0xE20A0, 0, g, u);
}
