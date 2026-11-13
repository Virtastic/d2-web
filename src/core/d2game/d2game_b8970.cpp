// 1.13c D2Game+0xB8970 -- npc menu test for the npcs 0x16F and 0xF6 over the quest 0x1A flags
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fb8970(NE& e, uint32_t eax0, uint32_t q, uint32_t id, uint32_t s0, uint32_t buf, uint32_t s2)
{
    const uint32_t game = e.rd32(q + 4), exp = e.rd32(game + 0x70);
    uint32_t r;
    if (id == 0x16f) {
        r = e.fl(buf, 0x1a, 0);
        if (!r) { r = e.fl(buf, 0x1a, 0xf); if (!r && e.rd8(q + 0xc) == 1) return 1; }
        if (!exp) { r = e.fl(buf, 0x1a, 0); if (r) { r = e.fl(buf, 0x1a, 7); if (r) return NE::lo(r, 1); } }
        if (!exp) return 0;
        r = e.fl(buf, 0x1a, 0);
        if (!r) return 0;
        r = e.fl(buf, 0x1a, 9);
        return r ? NE::lo(r, 0) : 1;
    }
    if (id == 0xf6) {
        if (!exp) { r = e.fl(buf, 0x1a, 0); if (r) { r = e.fl(buf, 0x1a, 6); if (r) return NE::lo(r, 1); } }
        if (!exp) return NE::lo(game, 0);
        r = e.fl(buf, 0x1a, 0);
        if (!r) return 0;
        r = e.fl(buf, 0x1a, 8);
        return r ? NE::lo(r, 0) : 1;
    }
    return NE::lo(eax0, 0);
}
}
