// 1.13c D2Client+0x446B0 -- [F46C8] = 8; when [11C3A0] and [11C3E0] are set: [11C3FC] = 1, bits = 8 if [11C3E8], |= 8 if [11C3EC], |= 4 if [11C3F0];
// when the pointer [11BBFC] is non-null and *it == 0: 1BF20(4, [11B828], [11B824], bits) (stdcall, host seam).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
struct D2c446B0G { int32_t gf46c8, g11c3a0, g11c3e0, g11c3e8, g11c3ec, g11c3f0, g11c3fc, g11b828, g11b824; };
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_446b0(D2cHost h, D2c446B0G* g, const int32_t* p11bbfc) {
    g->gf46c8 = 8;
    if (!g->g11c3a0 || !g->g11c3e0) return;
    uint32_t bits = 0;
    g->g11c3fc = 1;
    if (g->g11c3e8) bits = 8;
    if (g->g11c3ec) bits |= 8;
    if (g->g11c3f0) bits |= 4;
    if (p11bbfc && *p11bbfc == 0) { D2cCall c = {}; c.rva = 0x1BF20; c.s[0] = 4; c.s[1] = (uint32_t)g->g11b828; c.s[2] = (uint32_t)g->g11b824; c.s[3] = bits; h(&c); }
}
