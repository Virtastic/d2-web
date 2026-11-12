// 1.13c D2Common+0x71AA0 -- name callback (ecx = text, edx = out kind; stack: 1, 3 required): any other argument pair answers 0 with nothing written. A name found through Fog 10217 in the link table at image offset 0xA0AA8 stores kind 1 and answers the index; otherwise kind 0 is stored and "base" answers 1, "mod" answers 2 and anything else (after the "accr" comparison) answers 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_a0aa8;
namespace d2common {
uint32_t f71aa0(GE& e, uint32_t s, uint32_t out, uint32_t a, uint32_t b)
{
    if (a != 1 || b != 3) return 0;
    if (g_d2common_a0aa8) {
        const uint32_t v = e.h(0x923a, 0, 0, 0, 0, 0, g_d2common_a0aa8, s, 0);
        if ((int32_t)v >= 0) { e.wr32(out, 1); return v; }
    }
    if (!e.h(0x9204, 0, 0, 0, 0, 0, s, 0xB208f4ecu, 0x20)) { e.wr32(out, 0); return 1; }
    if (!e.h(0x9204, 0, 0, 0, 0, 0, s, 0xB208f4e8u, 0x20)) { e.wr32(out, 0); return 2; }
    e.h(0x9204, 0, 0, 0, 0, 0, s, 0xB208f4e0u, 0x20);
    e.wr32(out, 0);
    return 0;
}
}
