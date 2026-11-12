// 1.13c D2Common+0x5E520 -- name callback (ecx = text, edx = out kind; stack: 1, argument kind 3..6): kinds 3 and 6 look the name up through Fog 10217 in the link table at image offset 0xA0A74, kind 4 in the one at 0xA0A40 and kind 5 in the one at 0xA0AA8; a hit stores kind 1 and answers the index. A kind 5 miss stores kind 0 and answers 1 for "base", 2 for "mod" and 0 otherwise; a kind 4 miss answers the Fog 10213 code lookup of the space-padded four-character name in the table at 0x9FF34; every other case answers that lookup in the table at 0x9FF14. Those store kind 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_a0a74;
__attribute__((weak)) uint32_t g_d2common_a0a40;
__attribute__((weak)) uint32_t g_d2common_a0aa8;
__attribute__((weak)) uint32_t g_d2common_9ff14;
__attribute__((weak)) uint32_t g_d2common_9ff34;
namespace d2common {
static uint32_t pad4(GE& e, uint32_t s) {
    uint32_t code = 0x20202020u;
    for (uint32_t k = 0; k < 4; k++) {
        const uint32_t c = e.rd8(s + k);
        if (!c) break;
        code = (code & ~(0xffu << (8 * k))) | (c << (8 * k));
    }
    return code;
}
uint32_t f5e520(GE& e, uint32_t s, uint32_t out, uint32_t a, uint32_t b)
{
    if (a == 1 && (b == 3 || b == 6)) {
        const uint32_t g = g_d2common_a0a74;
        if (g) {
            const uint32_t v = e.h(0x923a, 0, 0, 0, 0, 0, g, s, 0);
            if ((int32_t)v >= 0) { e.wr32(out, 1); return v; }
        }
    }
    else if (a == 1 && b == 4) {
        {
            const uint32_t g = g_d2common_a0a40;
            if (g) {
                const uint32_t v = e.h(0x923a, 0, 0, 0, 0, 0, g, s, 0);
                if ((int32_t)v >= 0) { e.wr32(out, 1); return v; }
            }
        }
        {
            const uint32_t v = e.h(0x9240, 0, 0, 0, 0, 0, g_d2common_9ff34, pad4(e, s), 1);
            e.wr32(out, 0);
            return v;
        }
    } else if (a == 1 && b == 5) {
        {
            const uint32_t g = g_d2common_a0aa8;
            if (g) {
                const uint32_t v = e.h(0x923a, 0, 0, 0, 0, 0, g, s, 0);
                if ((int32_t)v >= 0) { e.wr32(out, 1); return v; }
            }
        }
        if (!e.h(0x9204, 0, 0, 0, 0, 0, s, 0xB208f4ecu, 0x20)) { e.wr32(out, 0); return 1; }
        if (!e.h(0x9204, 0, 0, 0, 0, 0, s, 0xB208f4e8u, 0x20)) { e.wr32(out, 0); return 2; }
        e.h(0x9204, 0, 0, 0, 0, 0, s, 0xB208f4e0u, 0x20);
        e.wr32(out, 0);
        return 0;
    }
        const uint32_t v = e.h(0x9240, 0, 0, 0, 0, 0, g_d2common_9ff14, pad4(e, s), 1);
    e.wr32(out, 0);
    return v;
}
}
