// 1.13c D2Game+0x56EF0 -- npc availability test of the quest 0x28: one flag per npc id
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
static uint32_t areaOpen(NE& e, uint32_t game) {   // 0x56640: the quest 0x23 holds its area open (data +0x48 set, +0x44 clear)
    const uint32_t n = e.quest(game, 0x23);
    if (!n) return 0;
    const uint32_t d = e.rd32(n + 0x18);
    return e.rd8(d + 0x48) && !e.rd32(d + 0x44) ? 1 : 0;
}
uint32_t f56ef0(NE& e, uint32_t q, uint32_t id, uint32_t s0, uint32_t buf, uint32_t s2)
{
    uint32_t eax;
    if ((int32_t)id >= 0x219 && (int32_t)id <= 0x21b) {
        eax = areaOpen(e, e.rd32(q + 4));
        if (eax && e.rd8(q + 0xc) == 1) { eax = e.rd32(q + 0x18); if (!e.rd8(eax + 0x90)) return NE::lo(eax, 1); }
    }
    eax = e.fl(buf, 0x28, 0);
    if (!eax) return NE::lo(eax, 0);
    eax = id - 0x1ff;
    uint32_t f;
    switch (eax) {
    case 0: f = 4; break;
    case 1: f = 9; break;
    case 2: f = 6; break;
    case 4: f = 8; break;
    case 10: f = 7; break;
    case 9: eax = e.fl(buf, 0x28, 10); if (eax) return NE::lo(eax, 0); f = 5; break;
    default: return NE::lo(eax, 0);
    }
    eax = e.fl(buf, 0x28, f);
    return NE::lo(eax, eax ? 0 : 1);
}
}
