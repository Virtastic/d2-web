// 1.13c D2Client+0x8AE00 particle field draw / step (no arguments, void). Globals (addresses in G): init flag 0x11BF24, seed
// pointer 0xE1304 (-> lo, hi), 0x10B9CC (ECX of 0xC9B2), colour bytes 0x10B440[256], last tick 0x10B598, y 0x10B5A0[256], palette
// bytes 0x10EAE0[8], x 0x10EAF0[256], dx 0x10EEF0[256]. First call (flag clear): 0xC634 (ECX seed), 0xC9B2 (ECX [0x10B9CC]),
// 0xC3B2 (ECX seed, EDX that result); for acc = 0, 0x80, .. 0x380 the base acc / 7 + 0x80 and three draws of the seed
// (lo * 0x6AC690C5 + hi, 64-bit; (lo & 0x3F) - 0x20 + base, capped at 0xFF) go to 0xD390 (r, g, b; stdcall), its AL into the
// palette; 0x8AC40 (EAX 0) 256 times; flag := 1. Every call: 0xD102 (x, y, x, y, colour, 0xFF) per particle; then GetTickCount
// (import 0xCEF5C): now - last > 0x28 (unsigned) steps x += dx per particle, 0x8AC40 (EAX 1) on a negative sum, last := now.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c8ae00G { uint32_t flag, seedp, b9cc, col, tick, y, pal, x, dx; };
static uint32_t rd32(uint32_t a) { uint32_t x; memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
static void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_8ae00(const D2cSvc& v, const D2c8ae00G* g) {
    if (rd32(g->flag) == 0) {
        d2c_call(v, 0xc634, 0, rd32(g->seedp), 0, 0, 0);
        const uint32_t e = d2c_call(v, 0xc9b2, 0, rd32(g->b9cc), 0, 0, 0);
        d2c_call(v, 0xc3b2, 0, rd32(g->seedp), e, 0, 0);
        uint32_t i = 0;
        for (int32_t acc = 0; acc < 0x400; acc += 0x80, ++i) {
            const int32_t base = acc / 7 + 0x80;
            int32_t c[3];
            for (int k = 0; k < 3; ++k) {
                const uint32_t sp = rd32(g->seedp);
                const uint64_t n = (uint64_t)rd32(sp) * 0x6ac690c5u + rd32(sp + 4);
                wr32(sp, (uint32_t)n); wr32(sp + 4, (uint32_t)(n >> 32));
                c[k] = (int32_t)((uint32_t)n & 0x3f) - 0x20 + base;
                if (c[k] > 0xff) c[k] = 0xff;
            }
            const uint32_t al = d2c_call(v, 0xd390, 0, 0, 0, 0, 0, (uint32_t)c[0], (uint32_t)c[1], (uint32_t)c[2]);
            *(uint8_t*)(uintptr_t)(g->pal + i) = (uint8_t)al;
        }
        for (int n = 0; n < 0x100; ++n) d2c_call(v, 0x8ac40, 0, 0, 0, 0, 0);
        wr32(g->flag, 1);
    }
    for (uint32_t i = 0; i < 0x100; ++i) {
        const uint32_t x = rd32(g->x + 4 * i), y = rd32(g->y + 4 * i), c = *(const uint8_t*)(uintptr_t)(g->col + i);
        d2c_call(v, 0xd102, 0, 0, 0, 0, 0, x, y, x, y, c, 0xff);
    }
    const uint32_t now = d2c_call(v, 0xcef5c, 0, 0, 0, 0, 0);
    if (now - rd32(g->tick) > 0x28) {
        for (uint32_t i = 0; i < 0x100; ++i) {
            const int32_t s = (int32_t)(rd32(g->x + 4 * i) + rd32(g->dx + 4 * i));
            wr32(g->x + 4 * i, (uint32_t)s);
            if (s < 0) d2c_call(v, 0x8ac40, 1, 0, 0, 0, 0);
        }
        wr32(g->tick, now);
    }
}
