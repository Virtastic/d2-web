// src/core/d2game/d2game_5e940.cpp -- 1.13c D2Game+0x5E940 transcribed from the cached Ghidra decompile.
// Callees not implemented in src/ are seams on D2Game5E940Env, in retail argument order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint8_t* P(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, P(a) + o, 4); return v; }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy(P(a) + o, &v, 4); }

struct D2Game5E940Env {
    uint32_t (*o10331)();                          // D2Common #10331 (result is the in_EAX of the later calls)
    void     (*sub21130)(uint32_t eax);            // 0x1130
    void     (*sub21100)(uint32_t eax);            // 0x1100
    uint32_t (*o11153)(uint32_t, uint32_t);        // #11153
    uint32_t (*o10826)();                          // #10826
    uint8_t  (*o10459)();                          // #10459
    void     (*subE0110)();                        // 0xE0110
    void*    (*o10045)(const char*, int, int);     // #10045 alloc
    uint32_t (*o10688)(uint32_t);                  // #10688
    void     (*o10445)(uint32_t);                  // #10445
    void     (*sub2F300)();                        // 0x2F300
    void     (*subE0F40)(uint32_t esi, uint32_t unit, uint32_t v, uint32_t game); // 0xE0F40
    void     (*subAEA80)(uint32_t game, uint32_t eax, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t); // 0x8EA80
};

// 1.13c D2Game+0x5E940 (ECX = game)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5E940(uint32_t game, uint32_t esi, const D2Game5E940Env* e)
{
    uint32_t h = rd32(game, 0x10f4);
    if (!h) return;
    uint32_t n = rd32(h, 0);
    if (!n) return;
    while (rd32(n, 0) != 0x21) { n = rd32(n, 0xf4); if (!n) return; }
    if (P(n)[9] == 0 || P(rd32(n, 0x18))[0xa4] != 0) return;

    uint32_t eax = e->o10331();
    e->sub21130(eax);
    e->sub21100(eax);
    uint32_t g = rd32(game, 0x1c);
    uint32_t u = e->o11153(g, 2);
    if (!u) {
        uint32_t q = rd32(n, 4);
        e->subAEA80(q, eax, 7, rd32(q, 0xa8) + 0x19, 0, 0, 0);
        return;
    }
    wr32(u, 0, 2);
    wr32(u, 4, 0x22e);
    wr32(u, 8, rd32(game, 0x1c));
    wr32(u, 0x80, game);
    uint32_t f = rd32(u, 0xc8);
    f = rd32(game, 0x70) ? (f | 0x2000000) : (f & 0xfdffffff);
    wr32(u, 0xc8, f | 0x4000000);
    uint32_t r = e->o10826();
    uint8_t b = e->o10459();
    P(u)[0x18] = b;
    wr32(u, 0x1c, rd32(game, 0xbc + (uint32_t)b * 4));
    wr32(u, 0x74, 0);
    e->subE0110();
    wr32(u, 0xc4, rd32(u, 0xc4) | 0x10);
    wr32(u, 0xd0, 0xb);
    int32_t c = (int32_t)rd32(game, 0x98) + 1;
    if (c == -1) c = 1;
    wr32(game, 0x98, (uint32_t)c);
    uint32_t m = (uint32_t)(uintptr_t)e->o10045("..\\Source\\D2Game\\UNIT\\SUnit.cpp", 0x265, 0);
    wr32(u, 0x14, m);
    std::memset(P(m), 0, 0x38);
    wr32(m, 0, e->o10688(0x22e));
    wr32(u, 0x10, 0);
    e->o10445(u);
    e->sub2F300();
    e->subE0F40(esi, u, r, game);
    wr32(u, 0xc4, rd32(u, 0xc4) | 0x3000000);
    P(g)[0xa4] = 1;
    wr32(g, 0xa8, rd32(u, 0xc));
}
