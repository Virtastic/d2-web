// Seam for D2Game+0x5C2A0 ACT5Q2_SpawnSoldierGroup.
#pragma once
#include <cstdint>
struct D2Game5C2A0Env {
    uint32_t (*spawn)(const uint32_t* req);                           // 0xEEF10, ECX = 10-word request, returns the new unit or 0
    void (*arm)(uint32_t unit, uint32_t node, uint32_t game);         // 0x33F80, EDI = unit, ESI = list node, stack = game (ret 4)
};
uint32_t d2game_port_5c2a0(uint32_t q, uint32_t p2, uint32_t p3, const D2Game5C2A0Env* env);
