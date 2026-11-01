// src/core/d2game/d2game_1af80.cpp -- 1.13c D2Game.dll sub_1AF80 written from the disassembly.
// fastcall (ECX = pGame, EDX = pUnit), one stack word pCtx (callee pops it). Callees are an explicit seam.
#include <cstdint>
#include <cstring>
#include "../prng.h"

struct D2Game1AF80Env {
    uint32_t (*imp7b4)(uint32_t a, uint32_t b);                                           // import thunk 0xA7B4, 2 stack words
    uint32_t (*imp718)(uint32_t a);                                                       // 0xA718, 1 stack word
    uint32_t (*imp4ea)(uint32_t a);                                                       // 0xA4EA, 1 stack word
    uint32_t (*imp3a0)(uint32_t a);                                                       // 0xA3A0, 1 stack word
    void (*step)(uint32_t eax, uint32_t unit, uint32_t game, uint32_t v, uint32_t tgt);   // 0xF2380: EAX, ECX unit, EBX game, 4 stack words (v, tgt, 0, 0)
    uint32_t (*find)(uint32_t unit, uint32_t game, uint32_t out);                         // 0xB1BC0: EAX unit, ECX game, 1 stack word (&out)
    void (*ord)(uint32_t unit, uint32_t zero);                                            // import 0xA496, 2 stack words
    uint32_t (*record)(uint32_t cls);                                                     // 0x1320: EAX class, returns a record pointer
    void (*submit)(uint32_t game, uint32_t rec, uint32_t one);                            // 0x75E10: 3 stack words
    void (*done)(uint32_t game, uint32_t unit, uint32_t v);                               // 0xF22C0: EAX game, ESI unit, stack v
    void (*cmd)(uint32_t game, uint32_t unit, uint32_t ctx8, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);   // 0xF2540
};

static inline uint32_t rd32_1af80(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }
static inline int32_t rs16_1af80(uint32_t p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 2); return v; }
static inline uint32_t lowb_1af80(uint32_t eax, uint32_t b) { return (eax & ~0xffu) | b; }

// 1.13c D2Game+0x1AF80; gl = the module global at +0xF8BE0 (+0x10 is the flag mask)
extern "C" void d2game_1af80_port(uint32_t game, uint32_t unit, uint32_t ctx, uint32_t gl, const D2Game1AF80Env* e)
{
    const int32_t p14 = (int32_t)rd32_1af80(ctx, 0x14);
    const uint32_t flag = rd32_1af80(ctx, 0x18);
    const uint32_t tbl = rd32_1af80(ctx, 0x1c);
    const uint32_t tgt = rd32_1af80(ctx, 8);
    const uint32_t idx = ((const uint8_t*)(uintptr_t)game)[0x6d];
    const uint32_t i2 = idx * 2;
    auto tb = [&](uint32_t o) { return (uint32_t)((const uint8_t*)(uintptr_t)tbl)[o]; };
    auto s16 = [&](uint32_t o) { return rs16_1af80(tbl, o); };
    d2::Seed* seed = (d2::Seed*)(uintptr_t)(unit + 0x20);
    auto r100 = [&]() { return (int32_t)((uint32_t)d2::roll(*seed) % 100); };

    const uint32_t a = e->imp7b4(tgt, 0x20);
    if (a == 0 && p14 < s16(0x68 + i2) && r100() < s16(0x62 + i2)) {
        uint32_t eax = idx;
        if (s16(0x170) > 0) {
            const uint32_t v = e->imp718(tgt);
            eax = v;
            if ((int32_t)v >= s16(0x7a + i2)) { e->step(lowb_1af80(v, tb(0x180)), unit, game, (uint32_t)s16(0x170), tgt); return; }
        }
        if (s16(0x172) > 0) {
            const uint32_t v = e->imp718(unit);
            eax = v;
            if ((int32_t)v <= s16(0x80 + i2)) { e->step(lowb_1af80((uint32_t)s16(0x172), tb(0x181)), unit, game, (uint32_t)s16(0x172), tgt); return; }
        }
        if (s16(0x174) > 0) {
            const uint32_t t = e->imp4ea(tgt);
            const uint32_t u = e->imp3a0(tgt);
            eax = tgt;
            if ((int32_t)u > (int32_t)t || tgt == 0 || rd32_1af80(tgt, 0) != 0) {
                e->step(lowb_1af80(tgt, tb(0x182)), unit, game, (uint32_t)s16(0x174), tgt); return;
            }
        }
        if (s16(0x176) > 0 && tgt != 0 && rd32_1af80(tgt, 0) == 0) {
            const uint32_t ax = (uint32_t)s16(0x176) & 0xffffu;
            e->step((eax & 0xffff0000u) | (ax & 0xff00u) | tb(0x183), unit, game, (uint32_t)s16(0x176), tgt); return;
        }
    }
    if (flag) {
        if (r100() >= s16(0x56 + i2)) { e->done(game, unit, (uint32_t)s16(0x6e + i2)); return; }
        e->ord(unit, 0);
        uint32_t bl = 0x64;
        if (unit) {
            const uint32_t rec = e->record(rd32_1af80(unit, 4));
            if (rec && (((const uint8_t*)(uintptr_t)rec)[0x104] & rd32_1af80(gl, 0x10))) bl = 0x65;
        }
        uint32_t st[8] = {};
        st[0] = 4; st[1] = unit; st[2] = tgt; ((uint8_t*)st)[0x15] = (uint8_t)bl;
        e->submit(game, 0xB3000000u, 1);
        return;
    }
    if (s16(0x178) > 0 && s16(0x80 + i2) > 0) {
        uint32_t out = 0;
        const uint32_t r = e->find(unit, game, (uint32_t)(uintptr_t)&out);
        if (r != 0) {
            const int32_t x = r100();
            if (x < s16(0x80 + i2)) { e->step(lowb_1af80((uint32_t)s16(0x178), tb(0x184)), unit, game, (uint32_t)s16(0x178), r); return; }
        }
    }
    if (r100() >= s16(0x5c + i2)) { e->done(game, unit, (uint32_t)s16(0x74 + i2)); return; }
    e->cmd(game, unit, tgt, 2, 0, 0, 1, 0);
}
