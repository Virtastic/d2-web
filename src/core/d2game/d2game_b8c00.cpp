// 1.13c D2Game+0xB8C00 -- npc 0x198 greeting choice over the quest 0x1B flags and the quest 0x17 data
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fb8c00(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t npc = e.rd32(a + 8);
    if (!npc) return 0;
    const uint32_t id = e.rd32(npc + 4);
    if (id != 0x198) return id;
    const uint32_t buf = e.qbuf(e.rd32(a), e.rd32(a + 0xc));
    if (!e.fl(buf, 0x1b, 0) && !e.fl(buf, 0x1b, 1) && !e.fl(buf, 0x1b, 0xd)) return e.h(0x33EB0, 0, q, 0, 0, 0, e.rd32(a + 0x14), 0x198);
    const uint32_t n = e.quest(e.rd32(q + 4), 0x17);
    if (n) { const uint32_t nd = e.rd32(n + 0x18); if (e.rd8(nd + 0x14)) return NE::lo(nd, e.rd8(nd + 0x14)); }
    return e.h(0x33EB0, 1, q, 0, 0, 0, e.rd32(a + 0x14), 0x198);
}
}
