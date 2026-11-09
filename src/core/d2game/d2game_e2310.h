// 1.13c D2Game+0xE2310: unit tick-range scheduler. EBX = unit, stack: game, p2 (ret 8). Addresses are 32-bit retail words.
// Callees are seams: e6f0(node, game) is the register routine at +0x8E6F0 (EAX = node, EDI = game); ea80(game, unit, s0..s4) is +0x8EA80
// (ECX = game, EDX = unit, five stack words, top first); a5f8(a, b, out) is the stdcall import at +0xA5F8 filling *out.
#pragma once
#include <cstdint>
struct D2GameE2310Env {
    void (*e6f0)(uint32_t node, uint32_t game);
    uint32_t (*ea80)(uint32_t game, uint32_t unit, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
    void (*a5f8)(uint32_t a, uint32_t b, uint32_t* out);
};
uint32_t d2game_e2310_port(uint32_t ebx, uint32_t game, uint32_t p2, const D2GameE2310Env* e);
