// 1.13c D2Game+0x62E50 -- npc greeting choice by player class for the npcs 0x93, 0x94, 0x96, 0x9A
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f62e50(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t npc = e.rd32(a + 8), id = npc ? e.rd32(npc + 4) : 0xffffffffu, pl = e.rd32(a + 0xc), cls = pl ? e.rd32(pl + 4) : 0xffffffffu;
    uint32_t want;
    switch (id) {
    case 0x93: want = 2; break;
    case 0x94: want = 1; break;
    case 0x96: want = 0; break;
    case 0x9a: want = 4; break;
    default: return pl;
    }
    const uint32_t eax = e.h(0x69590, pl, 0, 0, 0, id, e.rd32(a));
    if (eax) return eax;
    return e.h(0x33EB0, cls == want ? 1 : 0, q, 0, 0, 0, e.rd32(a + 0x14), id);
}
}
