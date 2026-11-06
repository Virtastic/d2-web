// 1.13c Fog+0x36C6 ___freetlocinfo(p): release the sub-tables of a locale-info block (guest 32-bit layout), then the block.
// Callees go through the FogEnv seam: _free 0x1FAD, ___free_lc_time 0x8B9E, ___free_lconv_num 0x8D2E, ___free_lconv_mon 0x8D8D.
// g = { time Fog+0x30520, mon 0x30524, ctype 0x57120, num_dec 0x57124, num_thou 0x57128 } (the C-locale default tables).
#include <cstdint>
#include "fog_env.h"
struct FogFreeTlocGlobals { uint32_t time, mon, ctype, num_dec, num_thou; };
extern "C" void fog_36c6(const FogEnv* e, uint32_t p, const FogFreeTlocGlobals* g) {
    const uint32_t mon = fe_r32(p + 0x3c);
    if (mon != g->mon && mon != 0 && fe_r32(fe_r32(p + 0x2c)) == 0) {
        uint32_t a = fe_r32(p + 0x34);
        if (a && fe_r32(a) == 0 && a != g->num_dec) { fe_call(e, 0x1fad, 0, 0, 0, 0, 0, a); fe_call(e, 0x8d8d, 0, 0, 0, 0, 0, fe_r32(p + 0x3c)); }
        a = fe_r32(p + 0x30);
        if (a && fe_r32(a) == 0 && a != g->num_thou) { fe_call(e, 0x1fad, 0, 0, 0, 0, 0, a); fe_call(e, 0x8d2e, 0, 0, 0, 0, 0, fe_r32(p + 0x3c)); }
        fe_call(e, 0x1fad, 0, 0, 0, 0, 0, fe_r32(p + 0x2c));
        fe_call(e, 0x1fad, 0, 0, 0, 0, 0, fe_r32(p + 0x3c));
    }
    const uint32_t c = fe_r32(p + 0x40);
    if (c != g->ctype && c != 0 && fe_r32(c) == 0) {
        fe_call(e, 0x1fad, 0, 0, 0, 0, 0, c);
        fe_call(e, 0x1fad, 0, 0, 0, 0, 0, fe_r32(p + 0x44) - 0xfe);
    }
    const uint32_t t = fe_r32(p + 0x50);
    if (t != g->time && t != 0 && fe_r32(t + 0xb4) == 0) {
        fe_call(e, 0x8b9e, 0, 0, 0, 0, 0, t);
        fe_call(e, 0x1fad, 0, 0, 0, 0, 0, fe_r32(p + 0x50));
    }
    fe_call(e, 0x1fad, 0, 0, 0, 0, 0, p);
}
