// 1.13c D2Client+0x8A6C0: summit01 scrolling backdrop (3 rows x 4 tiles, frames cycle with time) + 10 drifting clouds.
// Called from 0x8B110 (ECX = a this value, handed on to the tick call only). Callees go through the injected dispatcher, keyed by RVA:
// 0xC1D2 Ordinal_10651 tick (stack: ECX), 0x2B420 cel loader (EAX = path, one stack word 0), 0xD096 Ordinal_10041 draw (6 stack words:
// cell block, x, y, colour, mode, palette), 0xD0AE Ordinal_10031 render mode, the one-time seed init 0xC634 (ECX = seed) / GetTickCount
// IAT slot 0xCEF5C / 0xC9B2 (ECX = tick) / 0xC3B2 (ECX = seed, EDX = the 0xC9B2 result), and 0x1330 rand (EAX = count, ECX = seed).
// Globals live in D2c8A6C0State (RVA = VA - 0x6FAB0000); the seed is the pair {lo, hi} behind the pointer at D2Client+0xE1308.
#include <cstdint>
#include "d2client_callee.h"
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
    int32_t  cloudPos[10];    // 0x10B9A0
    int32_t  cloudY[10];      // 0x10B548
    int32_t  cloudSpd[10];    // 0x10B570
    int32_t  viewW;           // 0xF9E14 (+0x170 -> scroll limit)
};
static int32_t smod(uint32_t v, uint32_t mask) {  // x & 0x80000000|mask, signed-fixed as the compiler emits
    uint32_t r = v & (0x80000000u | mask);
    if ((int32_t)r < 0) r = ((r - 1) | ~mask) + 1;
    return (int32_t)r;
}
static void step(uint32_t* seed) {
    uint64_t v = (uint64_t)seed[0] * 0x6ac690c5u + seed[1];
    seed[0] = (uint32_t)v; seed[1] = (uint32_t)(v >> 32);
}
static void draw(const D2cSvc& v, D2c8A6C0Cell* c, int32_t x, int32_t y, uint32_t color, uint32_t mode) {
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)c, (uint32_t)x, (uint32_t)y, color, mode, 0);
}
// 1.13c D2Client+0x8A6C0
void d2c_8a6c0(const D2cSvc& v, uint32_t ecx, D2c8A6C0State* s, uint32_t* seed) {
    if (s->gate) return;
    const uint32_t seedp = (uint32_t)(uintptr_t)seed;
    if (!(s->tickInit & 1)) { s->tickInit |= 1; s->tickBase = d2c_call(v, 0xc1d2, 0, 0, 0, 0, 0, ecx); }
    if (!s->baseInit) { s->baseInit = 1; s->tickBase = 0x274f; }
    s->delta = d2c_call(v, 0xc1d2, 0, 0, 0, 0, 0, ecx) - s->tickBase;
    int32_t d = (int32_t)s->delta;
    int32_t i3 = d + (int32_t)((uint32_t)(d >> 31) & 7u);
    uint32_t u4 = (uint32_t)(i3 >> 3);
    int32_t u8 = smod(u4, 0xff);
    uint32_t t = (uint32_t)((int32_t)(u4 + ((uint32_t)(i3 >> 31) & 0xffu)) >> 8);
    int32_t r = smod(t, 3);
    if (r < 0) {
        uint32_t u6 = (uint32_t)r - 1 | 0xfffffffcu;
        uint32_t u = u6 + 1;
        if ((int32_t)u < 0) u = u6 + 5 + ((0u - u - 1) & 0xfffffffcu);
        r = (int32_t)u;
    }
    u4 = (uint32_t)smod((uint32_t)r, 3);
    int32_t x0 = -u8;
    char nm[32] = "data\\global\\ui\\summit01";
    if (!s->summitCel) s->summitCel = d2c_call(v, 0x2b420, (uint32_t)(uintptr_t)nm, 0, 0, 0, 0, 0);
    char nm2[32] = "data\\global\\ui\\cloud01";
    if (!s->cloudCel) s->cloudCel = d2c_call(v, 0x2b420, (uint32_t)(uintptr_t)nm2, 0, 0, 0, 0, 0);
    D2c8A6C0Cell c = {};
    if (s->summitCel) {
        c.cel = s->summitCel;
        uint32_t f[4];
        for (int k = 0; k < 4; k++) f[k] = (uint32_t)smod(u4 + k, 3);
        f[0] = u4;
        for (int row = 0; row < 3; row++) {
            if (row == 2 && d2c_call(v, 0xd0ae, 0, 0, 0, 0, 0) != 2) break;
            for (int k = 0; k < 4; k++) {
                c.frame = f[k] + row * 4u;
                draw(v, &c, x0 + 0x100 * k, 0x100 * (row + 1), 0xffffffffu, 5);
            }
            if (x0 > 0) { c.frame = f[3] + row * 4u; draw(v, &c, x0 - 0x100, 0x100 * (row + 1), 0xffffffffu, 5); }
        }
    }
    if (s->cloudCel) {
        if (!s->cloudInit) {
            d2c_call(v, 0xc634, 0, seedp, 0, 0, 0);
            const uint32_t tk = d2c_call(v, 0xcef5c, 0, 0, 0, 0, 0);
            const uint32_t mix = d2c_call(v, 0xc9b2, 0, tk, 0, 0, 0);
            d2c_call(v, 0xc3b2, 0, seedp, mix, 0, 0);
            for (int i = 0; i < 10; i++) {
                s->cloudPos[i] = (int32_t)d2c_call(v, 0x1330, (uint32_t)s->viewW << 4, seedp, 0, 0, 0);
                step(seed); s->cloudY[i] = (int32_t)(seed[0] % 0xfa);
                step(seed); s->cloudSpd[i] = (int32_t)((seed[0] & 0xf) + 8);
            }
            s->cloudInit = 1;
        }
        for (int i = 0; i < 10; i++) {
            s->cloudPos[i] += s->cloudSpd[i];
            if ((s->viewW + 0x170) * 0x10 < s->cloudPos[i]) {
                s->cloudPos[i] = (int32_t)0xffffe900;
                step(seed); s->cloudY[i] = (int32_t)(seed[0] % 300);
            }
        }
        c = D2c8A6C0Cell{}; c.cel = s->cloudCel;
        for (int i = 0; i < 10; i++) {
            int32_t p = s->cloudPos[i];
            int32_t x = (p + (int32_t)((uint32_t)(p >> 31) & 0xfu)) >> 4;
            draw(v, &c, x, s->cloudY[i], 0xddddddddu, 3);
            c.frame = 1;  // retail stores 1 in the cell block's frame slot for the second cel (mov [esp+0x2c],1 at 0x8AC03)
            draw(v, &c, x + 0x100, s->cloudY[i], 0xddddddddu, 3);
            c.frame = 0;
        }
    }
}
