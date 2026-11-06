// 1.13c D2Game+0x6E400. Retail: __fastcall (ECX = game, EDX = unit), plain ret (no stack words).
// Callees: 0xA478 (stdcall 1, unit; result unused); 0x6AE60 (stdcall 1, game); 0x8EA80 (fastcall ECX = game, EDX = unit,
// stdcall 5: 0, value, 0, 0, 0; ret 0x14).
// Step order: mode 1 -> 2 on the unit; A478(unit); step the 64-bit LCG at game+0x10F0 (lo*0x6AC690C5 + hi);
// 6AE60(game); the value is game+0xA8 + 0xF + (new low dword % 0x23); 8EA80 with it.
#include <cstdint>
#include <emscripten.h>

struct D2game6e400Env {
    void (*a478)(uint32_t unit);
    void (*ae60)(uint32_t game);
    void (*eaea80)(uint32_t game, uint32_t unit, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2game_6e400(uint8_t* game, uint8_t* unit, const D2game6e400Env* env) {
    if (unit && *(uint32_t*)(unit + 0x10) == 1) *(uint32_t*)(unit + 0x10) = 2;
    env->a478((uint32_t)(uintptr_t)unit);
    uint32_t* seed = (uint32_t*)(uintptr_t)*(uint32_t*)(game + 0x10F0);
    env->ae60((uint32_t)(uintptr_t)game);
    const uint64_t next = (uint64_t)seed[0] * 0x6AC690C5u + seed[1];
    seed[0] = (uint32_t)next;
    seed[1] = (uint32_t)(next >> 32);
    const uint32_t slot = (uint32_t)next % 0x23u;
    const uint32_t value = *(uint32_t*)(game + 0xA8) + 0xFu + slot;
    env->eaea80((uint32_t)(uintptr_t)game, (uint32_t)(uintptr_t)unit, 0, value, 0, 0, 0);
}
