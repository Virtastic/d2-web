// src/core/d2game/d2game_181a0.cpp -- 1.13c D2Game.dll sub_181A0 written from the disassembly.
// fastcall (ECX = pGame, EDX = pUnit), one stack word pCtx (callee pops it). Callees are an explicit seam.
#include <cstdint>
#include <cstring>
#include "../prng.h"

struct D2Game181A0Env {
    void (*ord)(uint32_t unit, uint32_t zero);                                            // import 0xA496, 2 stack words
    uint32_t (*record)(uint32_t cls);                                                     // 0x1320: EAX class, returns a record pointer
    void (*submit)(uint32_t game, uint32_t rec, uint32_t one);                            // 0x75E10: 3 stack words (game, &rec, 1)
    void (*done)(uint32_t game, uint32_t unit, uint32_t v);                               // 0xF22C0: EAX game, ESI unit, stack v
    void (*cmd)(uint32_t game, uint32_t unit, uint32_t ctx8, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);   // 0xF2540
};

static inline uint32_t rd32_181a0(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }

// 1.13c D2Game+0x181A0; gl = the module global at +0xF8BE0 (a pointer to a table whose +0x14 word is the flag mask)
extern "C" void d2game_181a0_port(uint32_t game, uint32_t unit, uint32_t ctx, uint32_t gl, const D2Game181A0Env* e)
{
    const uint32_t ctx8 = rd32_181a0(ctx, 8);
    d2::Seed* seed = (d2::Seed*)(uintptr_t)(unit + 0x20);
    if (rd32_181a0(ctx, 0x18)) {
        if ((int32_t)((uint32_t)d2::roll(*seed) % 100) >= 0x5f) { e->done(game, unit, 0xa); return; }
        (void)d2::roll(*seed);        // second step; the flags then always take the 0xA496 path
        e->ord(unit, 0);
        uint32_t bl = 0x64;
        if (unit) {
            const uint32_t rec = e->record(rd32_181a0(unit, 4));
            if (rec && (((const uint8_t*)(uintptr_t)rec)[0x104] & rd32_181a0(gl, 0x14))) bl = 0x65;
        }
        uint32_t st[8] = {};
        st[0] = 5; st[1] = unit; st[2] = ctx8; ((uint8_t*)st)[0x15] = (uint8_t)bl;
        e->submit(game, 0xB3000000u, 1);
        return;
    }
    if ((int32_t)((uint32_t)d2::roll(*seed) % 100) >= 0x59) { e->done(game, unit, 0xa); return; }
    e->cmd(game, unit, ctx8, 2, 0, 0, 1, 0);
}
