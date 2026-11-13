// 1.13c D2Game+0xE7880 -- quest event handler: the 0x4e step opens state 1, the 0x4b talk step in state 2 moves to state 3 unless the quest flags say it is done
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe7880(NE& e, uint32_t q, uint32_t a)
{
    uint32_t eax = e.rd32(a + 0x14);
    if (e.rd32(a + 0x18) == 0x4e && e.rd8(q + 9) && !e.rd8(q + 0xc)) e.wr8(q + 0xc, 1);
    if (eax != 0x4b) return eax;
    if (NE::rd16(q + 0x9c)) { const uint32_t u = e.rd32(a + 0xc); eax = e.h(0x33830, u ? e.rd32(u + 0xc) : ~0u, q + 0x1c); }
    eax = NE::lo(eax, e.rd8(q + 0xc));
    const uint32_t c = e.rd8(q + 0xc);
    if (c == 2) {
        const uint32_t buf = e.qbuf(e.rd32(a), e.rd32(a + 0xc));
        eax = e.fl(buf, e.rd32(q + 0xe0), 0); if (eax == 1) return eax;
        eax = e.fl(buf, e.rd32(q + 0xe0), 0xf); if (eax == 1) return eax;
        e.h(0x35EB0, 3);
        return e.cb(e.rd32(q + 4), 0xE46F0, 0, 0);
    }
    if (c == 5) e.wr8(q + 0xa, 0);
    return eax;
}
}
