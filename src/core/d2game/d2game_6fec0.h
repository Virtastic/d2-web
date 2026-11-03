// Seam for D2Game+0x6FEC0 MISSILES_RemoveAll (register convention: EDI = game).
#pragma once
#include <cstdint>
struct D2Game6FEC0Env {
    void (*removeUnit)(uint32_t game, uint32_t unit);    // 0xE20A0 SUNIT_RemoveUnit: ECX = game, EDX = unit, plain ret
};
void d2game_port_6fec0(uint32_t game, const D2Game6FEC0Env* env);
