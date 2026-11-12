// 1.13c D2Common+0x7C640 -- name callback (ecx = text, edx = out kind; stack: 1, argument kind): kind 3 looks the name up through Fog 10217 in the link table at image offset 0x9FF48 and kind 4 in the one at 0xA0A40; a hit stores kind 1 and answers the index. A kind 3 miss answers the Fog 10213 code lookup of the space-padded four-character name in the table at 0x9FF14; everything else (a missing table, a kind 4 miss, other arguments) answers that lookup in the table at 0x9FF34. Those store kind 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_9ff48;
__attribute__((weak)) uint32_t g_d2common_9ff14;
__attribute__((weak)) uint32_t g_d2common_a0a40;
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
uint32_t f7c640(GE& e, uint32_t s, uint32_t out, uint32_t a, uint32_t b)
{
    if (a == 1 && b == 3) {
        const uint32_t g = g_d2common_9ff48;
        if (g) {
            const uint32_t v = e.h(0x923a, 0, 0, 0, 0, 0, g, s, 0);
            if ((int32_t)v >= 0) { e.wr32(out, 1); return v; }
            {
                const uint32_t v = e.h(0x9240, 0, 0, 0, 0, 0, g_d2common_9ff14, pad4(e, s), 1);
                e.wr32(out, 0);
                return v;
            }
                }
    } else if (a == 1 && b == 4) {
        const uint32_t g = g_d2common_a0a40;
        if (g) {
            const uint32_t v = e.h(0x923a, 0, 0, 0, 0, 0, g, s, 0);
            if ((int32_t)v >= 0) { e.wr32(out, 1); return v; }
        }
    }
        const uint32_t v = e.h(0x9240, 0, 0, 0, 0, 0, g_d2common_9ff34, pad4(e, s), 1);
    e.wr32(out, 0);
    return v;
}
}
