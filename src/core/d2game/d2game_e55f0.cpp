// 1.13c D2Game+0xE55F0 -- ECX = quest node, EDX = event (+0 game, +8 and +0xC records whose +4 word is the type, -1 when absent; +0x14 argument); no stack argument. The +8 type picks the wanted +0xC type: 0xF5 and 0x129 give 7, 0xFC gives 0, 0xFE gives 2, 0xFF gives 3, 0x108 gives 4; any other type returns the +0xC record. The worker 0x69590 (EAX = the +0xC record, ESI = the +8 type, stack = the game) answering non-zero returns that. Otherwise 0x33EB0 runs (EAX = 1 when the +0xC type equals the wanted one, else 0; ECX = node; stack = the +0x14 word, the +8 type) and its answer is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe55f0(GE& e, uint32_t node, uint32_t ev)
{
    const uint32_t r1 = e.rd32(ev + 8), r2 = e.rd32(ev + 0xc);
    const uint32_t c1 = r1 ? e.rd32(r1 + 4) : 0xffffffffu, c2 = r2 ? e.rd32(r2 + 4) : 0xffffffffu;
    uint32_t want;
    switch (c1) {
    case 0xf5: case 0x129: want = 7; break;
    case 0xfc: want = 0; break;
    case 0xfe: want = 2; break;
    case 0xff: want = 3; break;
    case 0x108: want = 4; break;
    default: return r2;
    }
    const uint32_t v = e.h(0x69590, r2, 0, 0, 0, c1, e.rd32(ev));
    if (v) return v;
    return e.h(0x33EB0, c2 == want ? 1u : 0u, node, 0, 0, 0, e.rd32(ev + 0x14), c1);
}
}
