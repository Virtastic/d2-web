// 1.13c D2Game+0x56C40 -- npc availability test: ids 0x203 and 0x216 against the quest flags 0 and 1
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f56c40(NE& e, uint32_t eax0, uint32_t q, uint32_t id, uint32_t s0, uint32_t buf, uint32_t s2)
{
    uint32_t eax = eax0; const uint32_t fid = e.rd32(q + 0xe0);
    if (id == 0x203) {
        if (e.rd8(q + 0xc) == 1) {
            eax = e.fl(buf, fid, 0);
            if (!eax) { eax = e.fl(buf, fid, 1); if (!eax) return NE::lo(eax, 1); }
            return NE::lo(eax, 0);
        }
        eax = e.fl(buf, fid, 1);
        return NE::lo(eax, eax ? 1 : 0);
    }
    if (id == 0x216) { eax = e.fl(buf, 0x24, 0); if (!eax) { eax = e.fl(buf, 0x24, 1); if (!eax) return NE::lo(eax, 1); } }
    return NE::lo(eax, 0);
}
}
