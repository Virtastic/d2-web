// 1.13c D2Game+0x59930 -- starts the quest 0x24 for the game when it is idle
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f59930(NE& e, uint32_t p)
{
    const uint32_t n = e.quest(e.rd32(p + 4), 0x24);
    uint32_t eax = e.rd32(e.rd32(p + 4) + 0x10f4);   // the walk leaves the list holder in EAX
    if (e.rd8(n + 9) && !e.rd8(n + 0xc)) {
        e.wr8(e.rd32(n + 0x18) + 0x90, 1);
        e.wr8(n + 0xc, 2); e.wr8(n + 0x14, 0); e.wr8(n + 0xb, 1);
        e.cb(e.rd32(n + 4), 0x58D40);
        eax = e.cb(e.rd32(n + 4), 0x57320);
    }
    return NE::lo(eax, 1);
}
}
