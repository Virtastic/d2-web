// D2Client+0x44A80: per-frame update tail called from 44B50. Callees go through D2cHost in retail order.
// Not-yet-recovered imports are tagged by ordinal (10331 = unresolved import; CE4A=#10007 CE50=#10135 CE62=#10047 CE6E=#10715 CE74=#11125).
// g: +0 tick97EC, +4 lastC03C, +8 cnt97F8, +12 flagC04C, +16 objBBFC, +20 objC3B8. now[0..2]: the three GetTickCount results in order.
#include <emscripten.h>
#include <cstdint>
#include "client_host.h"
static uint32_t cl(D2cHost h, uint32_t rva, uint32_t a = 0, uint32_t b = 0) { D2cCall c = { rva, 0, a, b, 0, 0, { 0, 0, 0, 0 } }; return h(&c); }
// 1.13c D2Client+0x44A80
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_44a80(D2cHost h, uint32_t* g, const uint32_t* now) {
    g[0] = now[0];
    cl(h, 0x440C0); cl(h, 0xA8110);
    cl(h, 0xCE74, g[4] ? cl(h, 10331, g[4]) : 0);
    cl(h, 0x53720);
    if (g[3] != 0 || now[1] >= g[1] + 600) { cl(h, 0x6D010); g[1] = now[2]; }
    if (cl(h, 0xCE50, g[5], g[4] ? cl(h, 10331, g[4]) : 0)) cl(h, 0xA6680);
    cl(h, 0xCE6E, cl(h, 0xCE62, g[5]));
    cl(h, 0x62AF0);
    g[2]++;
    uint32_t r = g[2] / 13;
    if (g[2] % 13 == 0) r = cl(h, 0xCE4A, g[5]);
    return r;
}
