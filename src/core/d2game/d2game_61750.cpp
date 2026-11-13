// 1.13c D2Game+0x61750 -- npc availability test of the quest 4: ids 0x94 and 0x109
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f61750(NE& e, uint32_t q, uint32_t id, uint32_t unit, uint32_t buf, uint32_t s2)
{
    const uint32_t f13 = e.fl(buf, 4, 0xd), f0 = e.fl(buf, 4, 0), f1 = e.fl(buf, 4, 1);
    uint32_t eax = f1;
    if (id == 0x94) {
        const uint32_t c = e.rd8(q + 0xc);
        if (c == 4 && !f0 && !f1) {
            const uint32_t r = e.h(0xDF780, unit);
            eax = e.h(0xE660, unit, 0, 0, r, 0, 0x20736b62);
            if (eax) return NE::lo(eax, 1);
        }
        eax = NE::lo(eax, c);
        if (c == 1 && !f0 && !f1) return NE::lo(eax, 1);
        if (c == 6 && f13 == 1 && !f0) return NE::lo(eax, 1);
        return NE::lo(eax, f1 ? 1 : 0);
    }
    if (id == 0x109) {
        const uint32_t data = e.rd32(q + 0x18), uid = unit ? e.rd32(unit + 0xc) : 0xffffffffu;
        const uint32_t n = NE::rd16(data + 0x134);
        for (uint32_t i = 0; i < n; i++) if (e.rd32(data + 0xb4 + 4 * i) == uid) return NE::lo(i, 1);
        eax = e.h(0x33800, 0, 0, data + 0x138, 0, uid);
        if (eax) return NE::lo(eax, 0);
        eax = f13;
        return NE::lo(eax, eax ? 1 : 0);
    }
    return NE::lo(eax, 0);
}
}
