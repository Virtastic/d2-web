// Probe for the host-string branch row: the four control globals come from blob gv, the host string from blob ip;
// each seam records its registers in the capture's hook order. The 0x9862 hook answers 0x71, 0x72, 0x73 (the last repeats).
#include <emscripten.h>
#include <cstdint>
#include "d2game_hooklog.h"
#include "../d2launch/d2launch_12560.h"

static D2HookLog* g_lg;
static const uint32_t* g_r;
static uint32_t g_n9862;
static uint32_t tg(uint32_t a) {  // the ip blob address as the capture records it
    return a == g_r[16] ? d2TagBlob(g_r, 1, (const void*)(uintptr_t)a) : a;
}
static uint32_t h97c6() { g_lg->rec(0x97c6); return g_r[16]; }
static void h996a(uint32_t c) { g_lg->rec(0x996a, 0, c); }
static uint32_t h9b32(uint32_t c) { g_lg->rec(0x9b32, 0, c); return 0x5150; }
static void h98c8(uint32_t c, uint32_t d) { g_lg->rec(0x98c8, 0, c, tg(d)); }
static void h9976(uint32_t c, uint32_t d) { g_lg->rec(0x9976, 0, c, tg(d)); }
static uint32_t h9862(uint32_t c, uint32_t d) {
    g_lg->rec(0x9862, 0, c, d);
    const uint32_t v = 0x71 + (g_n9862 < 2 ? g_n9862 : 2); ++g_n9862; return v;
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2launch_12560(uint32_t* r) {
    D2HookLog lg(r[14]);
    g_lg = &lg; g_r = r; g_n9862 = 0;
    const uint32_t* gv = (const uint32_t*)(uintptr_t)r[18];
    const D2l12560 s = { h97c6, h996a, h9b32, h98c8, h9976, h9862, gv[0], gv[1], gv[2], gv[3] };
    r[4] = d2launch_sub_12560(&s);
}
