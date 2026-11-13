// 1.13c D2Game+0xC4880 -- ECX = quest record q: the 0xF dwords at +0xA0 are cleared and bytes +0xB, +0xC set to 0, then the callbacks are installed (+0xA0 = 0xC2680, +0xCC = 0xC2720, +0xDC = data 0x105E08, +0xE8 = 0xC1960, +0xEC = 0xC25F0; +0xE0 = 0x1F; byte +0xA = 1). A 2-byte block comes from the allocator 0xA2E6 (ECX = [[q+4]+0x1C], EDX = 2; stack: source file string, line 0x96, 0), its pointer goes to q+0x18 and its first two bytes are zeroed. EAX leaves as the block.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc4880(GE& e, uint32_t q)
{
    for (uint32_t o = 0; o < 0x3c; o += 4) e.wr32(q + 0xa0 + o, 0);
    e.wr8(q + 0xc, 0);
    e.wr8(q + 0xb, 0);
    e.wr32(q + 0xa0, 0xB2000000u | 0xC2680);
    e.wr32(q + 0xcc, 0xB2000000u | 0xC2720);
    e.wr32(q + 0xdc, 0xB2000000u | 0x105E08);
    e.wr8(q + 0xa, 1);
    const uint32_t p = e.h(0xA2E6, 0, e.rd32(e.rd32(q + 4) + 0x1c), 2, 0, 0, 0xB2000000u | 0xFB1F8, 0x96, 0);
    e.wr32(q + 0x18, p);
    e.wr8(p, 0);
    e.wr8(p + 1, 0);
    e.wr32(q + 0xe0, 0x1f);
    e.wr32(q + 0xe8, 0xB2000000u | 0xC1960);
    e.wr32(q + 0xec, 0xB2000000u | 0xC25F0);
    return p;
}
}
