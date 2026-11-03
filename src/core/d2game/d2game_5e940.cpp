// src/core/d2game/d2game_5e940.cpp -- 1.13c D2Game+0x5E940 transcribed from the cached Ghidra decompile.
// Callees not implemented in src/ are seams on D2Game5E940Env, in retail argument order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint8_t* P(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, P(a) + o, 4); return v; }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy(P(a) + o, &v, 4); }

struct D2Game5E940Env {
    uint32_t (*o10331)(uint32_t unit);                  // D2Common #10331: unit room (stack = unit)
    uint32_t (*sub21130)(uint32_t ecxUnit);             // 0x1130: ECX = unit, returns a value kept for 2F300/E0F40
    uint32_t (*sub21100)(uint32_t ecxUnit);             // 0x1100: ECX = unit, returns a value kept for 2F300/E0F40
    uint32_t (*o11153)(uint32_t pool, uint32_t type);   // #11153 (stack pool, 2)
    uint32_t (*o10826)(uint32_t room);                  // #10826
    uint8_t  (*o10459)(uint32_t v);                     // #10459
    void     (*subE0110)(uint32_t esiGameD0, uint32_t ediUnit); // 0xE0110: ESI = &game[0xd0], EDI = unit
    void*    (*o10045)(uint32_t pool, uint32_t size, const char*, int, int); // #10045: ECX pool, EDX size, stack file/line/0
    uint32_t (*o10688)(uint32_t);                       // #10688 (A520)
    void     (*o10445)(uint32_t);                       // #10445 (A724)
    void     (*sub2F300)(uint32_t esiUnit, uint32_t counter, uint32_t room, uint32_t r1100, uint32_t r1130); // stack (counter, room, r1100, r1130)
    void     (*subE0F40)(uint32_t eaxRoom, uint32_t ecxR1130, uint32_t edxUnit, uint32_t r1100, uint32_t game, uint32_t one); // 0xE0F40
    void     (*subAEA80)(uint32_t ecxGame, uint32_t edxUnit, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t); // 0x8EA80
};

// 1.13c D2Game+0x5E940 (ECX = game, EAX = unit), checked against the disassembly. Returns nothing.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5E940(uint32_t game, uint32_t unit, const D2Game5E940Env* e)
{
    uint32_t h = rd32(game, 0x10f4);
    if (!h) return;
    uint32_t n = rd32(h, 0);
    if (!n) return;
    while (rd32(n, 0) != 0x21) { n = rd32(n, 0xf4); if (!n) return; }
    if (P(n)[9] == 0) return;
    uint32_t st = rd32(n, 0x18);
    if (P(st)[0xa4] != 0) return;

    uint32_t room = e->o10331(unit);
    uint32_t r1130 = e->sub21130(unit);
    uint32_t r1100 = e->sub21100(unit);
    uint32_t pool = rd32(game, 0x1c);
    uint32_t u = e->o11153(pool, 2);
    if (!u) {
        uint32_t q = rd32(n, 4);
        e->subAEA80(q, unit, 7, rd32(q, 0xa8) + 0x19, 0, 0, 0);
        return;
    }
    wr32(u, 0, 2);
    wr32(u, 4, 0x22e);
    wr32(u, 8, pool);
    wr32(u, 0x80, game);
    uint32_t f = rd32(u, 0xc8);
    f = rd32(game, 0x70) ? (f | 0x2000000) : (f & 0xfdffffff);
    wr32(u, 0xc8, f);
    wr32(u, 0xc8, f | 0x4000000);
    uint8_t b = e->o10459(e->o10826(room));
    P(u)[0x18] = b;
    wr32(u, 0x1c, rd32(game, 0xbc + (uint32_t)b * 4));
    wr32(u, 0x74, 0);
    e->subE0110((uint32_t)(game + 0xd0), u);
    wr32(u, 0xc4, rd32(u, 0xc4) | 0x10);
    wr32(u, 0xd0, 0xb);
    int32_t c = (int32_t)rd32(game, 0x98) + 1;
    if (c == -1) c = 1;
    wr32(game, 0x98, (uint32_t)c);
    uint32_t m = (uint32_t)(uintptr_t)e->o10045(pool, 0x38, "..\\Source\\D2Game\\UNIT\\SUnit.cpp", 0x265, 0);
    wr32(u, 0x14, m);
    std::memset(P(m), 0, 0x38);
    wr32(m, 0, e->o10688(0x22e));
    wr32(u, 0x10, 0);
    e->o10445(u);
    e->sub2F300(u, (uint32_t)c, room, r1100, r1130);
    e->subE0F40(room, r1130, u, r1100, game, 1);
    wr32(u, 0xc4, rd32(u, 0xc4) | 0x3000000);
    P(st)[0xa4] = 1;
    wr32(st, 0xa8, rd32(u, 0xc));
}
