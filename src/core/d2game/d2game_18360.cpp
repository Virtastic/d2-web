// src/core/d2game/d2game_18360.cpp -- 1.13c D2Game.dll sub_18360 written from the disassembly.
// fastcall (ECX = pGame, EDX = pUnit), one stack word pCtx (callee pops it). Callees are an explicit seam.
#include <cstdint>
#include <cstring>
#include "../prng.h"

struct D2Game18360Env {
    void (*ord)(uint32_t unit, uint32_t zero);                                            // import 0xA496, 2 stack words
    void (*submit)(uint32_t game, uint32_t rec, uint32_t one);                            // 0x75E10: 3 stack words (game, &rec, 1)
    void (*done)(uint32_t game, uint32_t unit, uint32_t v);                               // 0xF22C0: EAX game, ESI unit, stack v
    void (*cmd)(uint32_t game, uint32_t unit, uint32_t ctx8, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);   // 0xF2540: ECX game, EDX unit, 6 stack words
};

static inline uint32_t rd32_18360(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }

// 1.13c D2Game+0x18360
extern "C" void d2game_18360_port(uint32_t game, uint32_t unit, uint32_t ctx, const D2Game18360Env* e)
{
    const uint32_t ctx8 = rd32_18360(ctx, 8), flag = rd32_18360(ctx, 0x18), tbl = rd32_18360(ctx, 0x1c), p = rd32_18360(ctx, 0);
    const uint32_t d = rd32_18360(p, 0x14);
    const uint32_t i = ((const uint8_t*)(uintptr_t)game)[0x6d];
    auto sw = [&](uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)tbl + i * 2 + o, 2); return (uint32_t)(int32_t)v; };
    if (d) {
        if ((int32_t)rd32_18360(game, 0xa8) > (int32_t)d) {
            e->ord(unit, 0);
            uint32_t rec[8] = {};
            rec[1] = unit; rec[2] = ctx8; ((uint8_t*)rec)[0x15] = 0x64;
            e->submit(game, 0xB3000000u, 1);
            return;
        }
        e->done(game, unit, sw(0x5c)); return;
    }
    if (flag) {
        const uint32_t v = sw(0x6e) + rd32_18360(game, 0xa8);
        std::memcpy((uint8_t*)(uintptr_t)p + 0x14, &v, 4);
        e->done(game, unit, sw(0x5c)); return;
    }
    d2::Seed* seed = (d2::Seed*)(uintptr_t)(unit + 0x20);
    const uint32_t lo = (uint32_t)d2::roll(*seed);
    if ((int32_t)(lo % 100) >= (int32_t)sw(0x62)) { e->done(game, unit, sw(0x5c)); return; }
    e->cmd(game, unit, ctx8, 2, 0, 0, 1, 0);
}
