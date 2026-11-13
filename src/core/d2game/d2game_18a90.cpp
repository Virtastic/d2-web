// src/core/d2game/d2game_18a90.cpp -- 1.13c D2Game.dll sub_18A90 written from the disassembly.
// fastcall (ECX = pGame, EDX = pUnit), one stack word pCtx (callee pops it). Callees are an explicit seam.
#include <cstdint>
#include <cstring>
#include "../prng.h"

struct D2Game18A90Env {
    void (*ord)(uint32_t unit, uint32_t zero);                                            // import 0xA496, 2 stack words
    uint32_t (*record)(uint32_t cls);                                                     // 0x1320: EAX class, returns a record pointer
    void (*submit)(uint32_t game, uint32_t rec, uint32_t one);                            // 0x75E10: 3 stack words (game, &rec, 1)
    void (*done)(uint32_t game, uint32_t unit, uint32_t v);                               // 0xF22C0: EAX game, ESI unit, stack v
    void (*cmd)(uint32_t game, uint32_t unit, uint32_t ctx8, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);   // 0xF2540
    uint32_t (*check)(uint32_t unit);                                                     // 0xB0E90: ECX unit, returns a flag
    uint32_t (*range)(uint32_t seed, uint32_t n);                                         // 0x1180: EAX n, ECX seed, returns the roll
    void (*skill)(uint32_t unit, uint32_t skillId, uint32_t ctx8, uint32_t z0, uint32_t z1, uint32_t b);   // 0xF2380: ECX unit, EAX byte, 4 stack words
    void (*atk)(uint32_t ctx8, uint32_t game, uint32_t unit, uint32_t a, uint32_t b);     // 0xF1D90: EAX 4, ECX ctx8, 4 stack words
};

static inline uint32_t rd32_18a90(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }

// 1.13c D2Game+0x18A90; gl = the module global at +0xF8BE0 (a pointer to a table whose +0x14 word is the flag mask)
extern "C" void d2game_18a90_port(uint32_t game, uint32_t unit, uint32_t ctx, uint32_t gl, const D2Game18A90Env* e)
{
    const uint32_t dist = rd32_18a90(ctx, 0x14), flag = rd32_18a90(ctx, 0x18), tbl = rd32_18a90(ctx, 0x1c), ctx8 = rd32_18a90(ctx, 8);
    const uint32_t i = ((const uint8_t*)(uintptr_t)game)[0x6d];
    auto sw = [&](uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)tbl + i * 2 + o, 2); return (int32_t)v; };
    d2::Seed* seed = (d2::Seed*)(uintptr_t)(unit + 0x20);
    if (unit) { uint32_t f = rd32_18a90(unit, 0xc4) | 0xe; std::memcpy((uint8_t*)(uintptr_t)unit + 0xc4, &f, 4); }
    if (flag) {
        if ((int32_t)((uint32_t)d2::roll(*seed) % 100) < sw(0x56)) {
            e->ord(unit, 0);
            uint32_t bl = 0x64;
            if (unit) {
                const uint32_t rec = e->record(rd32_18a90(unit, 4));
                if (rec && (((const uint8_t*)(uintptr_t)rec)[0x104] & rd32_18a90(gl, 0x14))) bl = 0x65;
            }
            uint32_t st[8] = {};
            st[0] = 4; st[1] = unit; st[2] = ctx8; ((uint8_t*)st)[0x15] = (uint8_t)bl;
            e->submit(game, 0xB3000000u, 1);
            return;
        }
        e->done(game, unit, (uint32_t)sw(0x5c)); return;
    }
    int16_t t172; std::memcpy(&t172, (const uint8_t*)(uintptr_t)tbl + 0x172, 2);
    if (t172 >= 0 && e->check(unit) && (int32_t)dist < sw(0x62) && (int32_t)dist > 5 &&
        (int32_t)e->range(unit + 0x20, 100) < sw(0x68)) {
        e->skill(unit, (uint32_t)(int32_t)t172, ctx8, 0, 0, ((const uint8_t*)(uintptr_t)tbl)[0x181]);
        return;
    }
    if ((int32_t)((uint32_t)d2::roll(*seed) % 100) < sw(0x6e)) { e->cmd(game, unit, ctx8, 2, 0, 0, 1, 0); return; }
    if ((int32_t)((uint32_t)d2::roll(*seed) % 100) < sw(0x74)) { e->atk(ctx8, game, unit, 2, 0); return; }
    e->done(game, unit, (uint32_t)sw(0x7a));
}
