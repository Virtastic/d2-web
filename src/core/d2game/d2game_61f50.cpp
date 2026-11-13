// 1.13c D2Game+0x61F50 -- npc message choice of a quest whose state maps through the table {-1, 0, 1, 2}
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f61f50(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t npc = e.rd32(a + 8), id = npc ? e.rd32(npc + 4) : 0xffffffffu;
    const uint32_t buf = e.qbuf(e.rd32(a), e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    const uint32_t f0 = e.fl(buf, fid, 0);
    uint32_t eax = e.fl(buf, fid, 1);
    if (eax) return e.h(0x33EB0, 3, q, 0, 0, 0, e.rd32(a + 0x14), id);
    const uint32_t pl = e.rd32(a + 0xc), uid = pl ? e.rd32(pl + 0xc) : 0xffffffffu;
    const uint32_t n = NE::rd16(q + 0x9c);
    uint32_t i = 0;
    for (; i < n; i++) if (e.rd32(q + 0x1c + 4 * i) == uid) return e.h(0x33EB0, 4, q, 0, 0, 0, e.rd32(a + 0x14), id);
    const uint32_t c = e.rd8(q + 0xc);
    eax = NE::lo(i, c);
    if (!c || f0 == 1 || c >= 4) return eax;
    static const uint32_t msg[4] = { 0xffffffffu, 0, 1, 2 };
    eax = msg[c];
    if (eax == 0xffffffffu || eax >= 6) return eax;
    return e.h(0x33EB0, eax, q, 0, 0, 0, e.rd32(a + 0x14), id);
}
}
