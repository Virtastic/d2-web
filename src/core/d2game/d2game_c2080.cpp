// 1.13c D2Game+0xC2080 -- npc menu test of quest 0xC for the npcs 0x14B, 0xB1 and 0xC9
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fc2080(NE& e, uint32_t q, uint32_t id, uint32_t s0, uint32_t buf, uint32_t s2)
{
    uint32_t r = e.fl(buf, 0xc, 0), f = 0;
    if (r == 1) return 0;
    if (id == 0x14b) {
        r = e.fl(buf, 0xc, 1);
        if (r == 1) return 0;
        const uint32_t data = e.rd32(q + 0x18), w = NE::rd16(data + 0x40);
        if (!w) f = 7;
        else {
            if (w != 2) return NE::lo(w - 2, 0);
            if (!e.rd8(data + 0x46)) return 0;
            f = 8;
        }
    } else {
        if (id == 0xb1) { if (e.rd8(q + 0xc) != 1) return NE::lo(r, 0); }
        else if (id == 0xc9) { if (e.rd8(q + 0xc) != 2) return NE::lo(r, 0); }
        else return NE::lo(r, 0);
        r = e.fl(buf, 0xc, 1);
        if (r) return NE::lo(r, 0);
    }
    r = e.fl(buf, 0xc, f);
    return r ? NE::lo(r, 0) : 1;
}
}
