// 1.13c D2Game+0xBA1E0 -- message 0x67 on a quest in state 2: drops the player from the watch list and finishes the quest unless the player already has it
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fba1e0(NE& e, uint32_t eax0, uint32_t q, uint32_t a)
{
    if (e.rd32(a + 0x14) != 0x67) return eax0;
    uint32_t eax = eax0;
    const uint32_t pl = e.rd32(a + 0xc);
    if (NE::rd16(q + 0x9c)) eax = e.h(0x33830, (pl ? e.rd32(pl + 0xc) : 0xffffffffu), q + 0x1c);
    if (e.rd8(q + 0xc) != 2) return eax;
    const uint32_t buf = e.qbuf(e.rd32(a), pl), fid = e.rd32(q + 0xe0);
    eax = e.fl(buf, fid, 0);
    if (eax == 1) return eax;
    eax = e.fl(buf, fid, 1);
    if (eax == 1) return eax;
    e.h(0x35EB0, 3);
    eax = e.cb(e.rd32(q + 4), 0xb8e80);
    
    return eax;
}
}
