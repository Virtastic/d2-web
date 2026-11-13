// 1.13c D2Game+0x57F50 -- npc availability test of an act 5 quest: ids 0x200, 0x201 and 0x20F
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f57f50(NE& e, uint32_t eax0, uint32_t q, uint32_t id, uint32_t unit, uint32_t buf, uint32_t s2)
{
    uint32_t eax = eax0; const uint32_t fid = e.rd32(q + 0xe0);
    if (id == 0x201) {
        eax = e.fl(buf, fid, 1);
        if (eax) { eax = e.fl(buf, 0x25, 8); if (!eax) return NE::lo(eax, 1); }
        if ((eax = e.fl(buf, fid, 0))) return NE::lo(eax, 0);
        if ((eax = e.fl(buf, fid, 1))) return NE::lo(eax, 0);
        const uint32_t st = e.rd8(q + 0xc);
        if (st == 1) return NE::lo(eax, 1);
        if (st != 4) return NE::lo(eax, 0);
        eax = e.rd32(q + 0x18);
        return NE::lo(eax, e.rd32(eax + 0xd8) ? 0 : 1);
    }
    if (id == 0x20f) {
        if (!e.rd8(q + 9)) return NE::lo(eax, 0);
        if (e.rd8(q + 0xc) >= 5) return NE::lo(eax, 0);
        eax = e.h(0xE660, unit, 0, 0, e.rd32(q + 4), 0, 0x20656369);
        return NE::lo(eax, eax ? 0 : 1);
    }
    if (id == 0x200) {
        eax = e.fl(buf, fid, 1);
        if (!eax) return NE::lo(eax, 0);
        eax = e.fl(buf, 0x25, 9);
        return NE::lo(eax, eax ? 0 : 1);
    }
    return NE::lo(eax, 0);
}
}
