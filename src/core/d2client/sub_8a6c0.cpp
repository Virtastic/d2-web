// 1.13c D2Client+0x8A6C0: summit01 scrolling backdrop (3 rows x 4 tiles, frames cycle with time) + 10 drifting clouds.
// Called from 0x8B110. Imports/relocs are behind the Env; globals live in D2c8A6C0State (RVA = VA - 0x6FAB0000).
#include <cstdint>
#include <emscripten.h>
struct D2c8A6C0Cell { uint32_t frame; uint32_t pad[12]; uint32_t cel; uint32_t pad2[4]; };  // 0x48 bytes
static_assert(sizeof(D2c8A6C0Cell) == 0x48, "cell ctx");
struct D2c8A6C0State {
    uint32_t gate;        // 0x11C3A4: nonzero -> do nothing
    uint32_t tickInit;    // 0x123708 bit0
    uint32_t tickBase;    // 0x123704
    uint8_t  baseInit;    // 0x123428
    uint32_t delta;       // 0x12342C
    uint32_t summitCel;   // 0x11BF1C
    uint32_t cloudCel;    // 0x11BF20
    uint8_t  cloudInit;   // 0x11BF28
    uint32_t seedLo, seedHi;  // *PTR_DAT_6fb91308
    int32_t  cloudPos[10];    // 0x10B9A0
    int32_t  cloudY[10];      // 0x10B548
    int32_t  cloudSpd[10];    // 0x10B570
    int32_t  viewW;           // 0xF9E14 (+0x170 -> scroll limit)
};
struct D2c8A6C0Env {
    uint32_t (*tick)();                                   // Ordinal_10651
    uint32_t (*loadCel)(const char* path);                // D2Client+0x2B420
    void (*drawCel)(D2c8A6C0Cell*, int32_t x, int32_t y, uint32_t color, uint32_t mode, uint32_t pal);  // Ordinal_10041
    int32_t (*renderMode)();                              // Ordinal_10031
    void (*seedInit)();                                   // Ordinals 10936/GetTickCount/10995/10614 (one-time)
    uint32_t (*rand)(uint32_t* lo, uint32_t* hi);         // D2Client+0x1330 on the seed (count arg lost in decompile)
};
static int32_t smod(uint32_t v, uint32_t mask, uint32_t m) {  // x & 0x80000000|mask, signed-fixed as the compiler emits
    uint32_t r = v & (0x80000000u | mask);
    if ((int32_t)r < 0) r = ((r - 1) | ~mask) + 1;
    (void)m; return (int32_t)r;
}
static void step(D2c8A6C0State* s) {
    uint64_t v = (uint64_t)s->seedLo * 0x6ac690c5u + s->seedHi;
    s->seedLo = (uint32_t)v; s->seedHi = (uint32_t)(v >> 32);
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_8a6c0(D2c8A6C0State* s, const D2c8A6C0Env* e) {
    if (s->gate) return;
    if (!(s->tickInit & 1)) { s->tickInit |= 1; s->tickBase = e->tick(); }
    if (!s->baseInit) { s->baseInit = 1; s->tickBase = 0x274f; }
    s->delta = e->tick() - s->tickBase;
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
                s->cloudPos[i] = (int32_t)e->rand(&s->seedLo, &s->seedHi);
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
            e->drawCel(&c, x, s->cloudY[i], 0xddddddddu, 3, 0);
            e->drawCel(&c, x + 0x100, s->cloudY[i], 0xddddddddu, 3, 0);
        }
    }
}
