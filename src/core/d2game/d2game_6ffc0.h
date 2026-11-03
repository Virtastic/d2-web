// Seam for D2Game+0x6FFC0 PLRTRADE_AddItemTradePair (EAX = player unit, stack = game, id1, id2, ret 0xC).
#pragma once
#include <cstdint>
struct D2Game6FFC0Env {
    uint32_t (*playerData)(uint32_t unit);                                  // 0xA4C0 (D2Common ordinal thunk), one stack word = unit
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line);        // 0xA2E6 Fog alloc: ECX = pool, EDX = size, stack = file ("PlrTrade.cpp"), line, 0
};
void d2game_port_6ffc0(uint32_t player, uint32_t game, uint32_t id1, uint32_t id2, const D2Game6FFC0Env* env);
