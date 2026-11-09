// 1.13c D2Client+0x8A6C0: summit01 scrolling backdrop (3 rows x 4 tiles, frames cycle with time) + 10 drifting clouds.
// Called from 0x8B110. Imports/relocs are behind the Env; globals live in D2c8A6C0State (RVA = VA - 0x6FAB0000).
#include <cstdint>
#include <emscripten.h>
#include "sub_8a6c0.h"
static int32_t smod(uint32_t v, uint32_t mask, uint32_t m) {  // x & 0x80000000|mask, signed-fixed as the compiler emits
    uint32_t r = v & (0x80000000u | mask);
    if ((int32_t)r < 0) r = ((r - 1) | ~mask) + 1;
    (void)m; return (int32_t)r;
}
static void step(D2c8A6C0State* s) {
    uint64_t v = (uint64_t)s->seedLo * 0x6ac690c5u + s->seedHi;
    s->seedLo = (uint32_t)v; s->seedHi = (uint32_t)(v >> 32);
}
// 1.13c D2Client+0x1330 (EAX = n, ECX = seed): n < 1 -> 0, seed untouched; else step, power of two -> (n-1)&lo, else lo % n.
static uint32_t rand1330(D2c8A6C0State* s, int32_t n) {
    if (n < 1) return 0;
    step(s);
    const uint32_t un = (uint32_t)n;
    return (un & (un - 1)) ? s->seedLo % un : (un - 1) & s->seedLo;
}
// 1.13c D2Client+0x8A6C0 (ECX = self, passed on to the tick call)
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_8a6c0(D2c8A6C0State* s, const D2c8A6C0Env* e, uint32_t self) {
    if (s->gate) return;
    if (!(s->tickInit & 1)) { s->tickInit |= 1; s->tickBase = e->tick(self); }
    if (!s->baseInit) { s->baseInit = 1; s->tickBase = 0x274f; }
    s->delta = e->tick(self) - s->tickBase;
    int32_t d = (int32_t)s->delta;
    int32_t i3 = d + (int32_t)((uint32_t)(d >> 31) & 7u);
    uint32_t u4 = (uint32_t)(i3 >> 3);
    int32_t u8 = smod(u4, 0xff, 0);
    uint32_t t = (uint32_t)((int32_t)(u4 + ((uint32_t)(i3 >> 31) & 0xffu)) >> 8);
    int32_t r = smod(t, 3, 0);
    if (r < 0) {
        uint32_t u6 = (uint32_t)r - 1 | 0xfffffffcu;
        uint32_t u = u6 + 1;
        if ((int32_t)u < 0) u = u6 + 5 + ((0u - u - 1) & 0xfffffffcu);
        r = (int32_t)u;
    }
    u4 = (uint32_t)smod((uint32_t)r, 3, 0);
    int32_t x0 = -u8;
    if (!s->summitCel) s->summitCel = e->loadCel("data\\global\\ui\\summit01");
    if (!s->cloudCel) s->cloudCel = e->loadCel("data\\global\\ui\\cloud01");
    D2c8A6C0Cell c = {};
    if (s->summitCel) {
        c.cel = s->summitCel;
        uint32_t f[4];
        for (int k = 0; k < 4; k++) f[k] = (uint32_t)smod(u4 + k, 3, 0);
        f[0] = u4;
        for (int row = 0; row < 3; row++) {
            if (row == 2 && e->renderMode() != 2) break;
            for (int k = 0; k < 4; k++) {
                c.frame = f[k] + row * 4u;
                e->drawCel(&c, x0 + 0x100 * k, 0x100 * (row + 1), 0xffffffffu, 5, 0);
            }
            if (x0 > 0) { c.frame = f[3] + row * 4u; e->drawCel(&c, x0 - 0x100, 0x100 * (row + 1), 0xffffffffu, 5, 0); }
        }
    }
    if (s->cloudCel) {
        if (!s->cloudInit) {
            e->seedInit();
            for (int i = 0; i < 10; i++) {
                s->cloudPos[i] = (int32_t)rand1330(s, (int32_t)((uint32_t)s->viewW << 4));
                step(s); s->cloudY[i] = (int32_t)(s->seedLo % 0xfa);
                step(s); s->cloudSpd[i] = (int32_t)((s->seedLo & 0xf) + 8);
            }
            s->cloudInit = 1;
        }
        for (int i = 0; i < 10; i++) {
            s->cloudPos[i] += s->cloudSpd[i];
            if ((s->viewW + 0x170) * 0x10 < s->cloudPos[i]) {
                s->cloudPos[i] = (int32_t)0xffffe900;
                step(s); s->cloudY[i] = (int32_t)(s->seedLo % 300);
            }
        }
        c = D2c8A6C0Cell{}; c.cel = s->cloudCel;
        for (int i = 0; i < 10; i++) {
            int32_t p = s->cloudPos[i];
            int32_t x = (p + ((uint32_t)(p >> 31) & 0xfu)) >> 4;
            c.frame = 0; e->drawCel(&c, x, s->cloudY[i], 0xddddddddu, 3, 0);
            c.frame = 1; e->drawCel(&c, x + 0x100, s->cloudY[i], 0xddddddddu, 3, 0);
        }
    }
}
