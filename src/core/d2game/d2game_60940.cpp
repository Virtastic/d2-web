// 1.13c D2Game+0x60940 -- npc availability test of the quest 6: ids 0x109, 0x9B, 0x94, 0x96
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f60940(NE& e, uint32_t q, uint32_t id, uint32_t unit, uint32_t buf, uint32_t s2)
{
    const uint32_t uid = unit ? e.rd32(unit + 0xc) : 0xffffffffu, data = e.rd32(q + 0x18);
    uint32_t eax = unit;
    if (id == 0x109) {
        const uint32_t n = NE::rd16(data + 0x80);
        for (uint32_t i = 0; i < n; i++) if (e.rd32(data + 4 * i) == uid) return NE::lo(i, 1);
        eax = e.fl(buf, 6, 0);
        if (eax == 1) return NE::lo(eax, 0);
        if (e.rd8(q + 0xc) != 1) return NE::lo(eax, 0);
        eax = e.fl(buf, 6, 1);
        return NE::lo(eax, eax ? 0 : 1);
    }
    if (id == 0x9b) {
        eax = e.fl(buf, 6, 0);
        if (eax == 1) return NE::lo(eax, 0);
        eax = e.fl(buf, 6, 1);
        return NE::lo(1 - eax, eax == 1 ? 1 : 0);
    }
    if (id == 0x94 || id == 0x96) {
        eax = e.h(0x33800, 0, 0, data + (id == 0x94 ? 0x84 : 0x108), 0, uid);
        return NE::lo(eax, eax ? 1 : 0);
    }
    return NE::lo(eax, 0);
}
}
