// 1.13c D2Client+0xB41F0: per-tick ambient spawner for a unit.
// u = 32-bit unit words (u[0] type, u[8..9] 64-bit LCG seed, u[0xB] data ptr/word). Ordinals go through the host as 0x80000000|ordinal.
// tab = 4 x {thresh, divA, divB} ints at D2Client+0xD3610; g = {tick,0,6000,0} at +0x10B9CC; flag20 = int at +0x11BEFC.
#include "client_host.h"
#include <emscripten.h>
static uint32_t H(D2cHost h, uint32_t rva, uint32_t a, uint32_t b = 0) { D2cCall k = { rva, a, b, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }; return h(&k); }
// seam pseudo-calls: 0xFFFF0001 = load dword [eax], 0xFFFF0002 = store dword [eax] = ecx
static uint32_t LD(D2cHost h, uint32_t a) { return H(h, 0xFFFF0001u, a); }
static void ST(D2cHost h, uint32_t a, uint32_t v) { H(h, 0xFFFF0002u, a, v); }
static void rnd(uint32_t* u, uint32_t& w1, uint32_t& w2) {
    uint64_t a = (uint64_t)u[8] * 0x6ac690c5u + u[9];
    uint64_t b = (a & 0xffffffffu) * 0x6ac690c5u + (a >> 32);
    u[8] = (uint32_t)b; u[9] = (uint32_t)(b >> 32);
    w1 = (uint32_t)a; w2 = (uint32_t)b;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b41f0(D2cHost h, uint32_t* u, const int32_t* tab, uint32_t* g, int32_t* flag20, uint32_t tick) {
    const uint32_t up = (uint32_t)(uintptr_t)u;
    int32_t n = (int32_t)H(h, 0x80000000u | 10737, up);
    if (n == 0x145) { g[0] = tick; g[1] = 0; g[2] = 6000; g[3] = 0; *flag20 = 0x14; }
    int i = 0;
    while (i < 4 && tab[i * 3] >= n) i++;   // retail walks rows while thresh >= n (bounded by table end)
    if (i > 3) i = 3;                        // ponytail: retail reads one row past the end here; clamp
    const bool dat = u[0] == 2 || (u[0] > 3 && u[0] < 6);
    int32_t x = dat ? (int32_t)LD(h, u[0xB] + 0xC) : u[0xB] ? (int32_t)H(h, 0x80000000u | 10867, u[0xB]) : 0;
    int32_t y = dat ? (int32_t)LD(h, u[0xB] + 0x10) : u[0xB] ? (int32_t)H(h, 0x80000000u | 10750, u[0xB]) : 0;
    for (int pass = 0; pass < 2; pass++) {
        if (n % tab[i * 3 + 1 + pass] != 0) continue;
        uint32_t s[23] = { 0 }, w1, w2;
        s[0] = 0x2001; s[1] = H(h, 0xA60D0, 0); s[4] = pass ? 0x16c : 0x2a;
        s[10] = H(h, 0x80000000u | 10240, up); s[12] = H(h, 0x80000000u | 11029, up);
        rnd(u, w1, w2);
        s[5] = (uint32_t)x - 0x14 + w1 % 0x28; s[6] = w2 % 0x28 - 0x14 + (uint32_t)y;
        uint32_t p = H(h, 0xB30F0, (uint32_t)(uintptr_t)s);
        if (pass && p) {
            uint32_t d = LD(h, p + 0x54) ? LD(h, LD(h, p + 0x54) + 0x30) : 0;
            if (d) { ST(h, d + 4, 0); ST(h, d + 8, 0); ST(h, d + 0xC, 0x4b000); }
            d = LD(h, p + 0x54) ? LD(h, LD(h, p + 0x54) + 0x30) : 0;
            if (d) { ST(h, d + 0x10, 0); ST(h, d + 0x14, 0); ST(h, d + 0x18, 0xffffb000u); }
            H(h, 0x80000000u | 10558, p, 0xf); H(h, 0x80000000u | 10970, p, 0xf); H(h, 0x80000000u | 10154, p, 0x80);
        }
    }
    H(h, 0xB17C0, up);
}
