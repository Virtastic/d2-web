// 1.13c D2Game+0x575F0 -- npc greeting chooser: asks for one message by the quest flags, the npc id and the quest state
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f575f0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18), game = e.rd32(q + 4);
    const uint32_t buf = e.qbuf(game, e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    uint32_t eax = e.fl(buf, fid, 0);
    const uint32_t npc = e.rd32(a + 8), id = npc ? e.rd32(npc + 4) : ~0u;
    if (!e.rd8(q + 9)) return eax;
    if (eax) {
        const uint32_t v1 = e.fl(buf, fid, 0xd);
        eax = id - 0x1ff;
        uint32_t f;
        switch (eax) {
        case 0: f = 5; break;
        case 9: f = 6; break;
        case 1: f = 7; break;
        case 2: f = 8; break;
        case 4: f = 9; break;
        default: return eax;
        }
        eax = e.fl(buf, fid, f);
        if (!eax) return e.h(0x33EB0, 5, q, 0, 0, 0, e.rd32(a + 0x14), id);
        if (!v1) return v1;
        return e.h(0x33EB0, 3, q, 0, 0, 0, e.rd32(a + 0x14), id);
    }
    if ((int32_t)id >= 0x219 && (int32_t)id <= 0x21b) {
        eax = e.rd8(data + 0x10);
        if (!eax) eax = e.h(0x33EB0, 4, q, 0, 0, 0, e.rd32(a + 0x14), id);
    }
    // the state's message, a module table of dwords indexed by the state byte
    static const uint32_t msg[7] = { ~0u, 0, 1, 2, 3, 4, 0 };
    const uint32_t st = e.rd8(q + 0xc);
    eax = st < 7 ? msg[st] : ~0u;
    if (eax == ~0u || eax >= 9) return eax;
    return e.h(0x33EB0, eax, q, 0, 0, 0, e.rd32(a + 0x14), id);
}
}
