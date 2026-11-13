// 1.13c D2Game+0x61540 -- restores the quest 4 state from the player flags and the two quest items the player holds
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f61540(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a), buf = e.qbuf(game, e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    uint32_t eax;
    const uint32_t data = e.rd32(q + 0x18);
    eax = e.fl(buf, 4, 0);
    if (eax == 1) {
        const uint32_t n = e.quest(e.rd32(q + 4), 4);
        if (n) e.wr8(e.rd32(n + 0x18) + 0x52, 1);
        const uint32_t holder = e.rd32(e.rd32(q + 4) + 0x10f4);
        e.setf(e.rd32(holder + 0xc), 4, 0xd);
        e.wr32(data + 0x54, 3); e.wr8(data + 0x4c, 1); e.wr8(data + 0x4d, 1);
    } else if ((eax = e.fl(buf, 4, 0xf)) == 1) {
        e.h(0x60590, e.rd32(q + 4));
        e.h(0x605C0, e.rd32(q + 4));
        e.wr32(data + 0x54, 3); e.wr8(data + 0x4c, 1); e.wr8(data + 0x4d, 1);
    } else if ((eax = e.fl(buf, 4, 4)) == 1) {
        e.wr8(q + 0xb, 4); e.wr8(q + 0xc, 5);
        e.wr8(data + 0x4c, 1); e.wr8(data + 0x4e, 1); e.wr8(data + 0x4d, 1); e.wr8(data + 0x78, 1); e.wr32(data + 0x58, 1);
    } else if ((eax = e.fl(buf, 4, 3)) == 1) {
        e.wr8(q + 0xc, 3); e.wr8(q + 0xb, 1);
    } else if ((eax = e.fl(buf, 4, 2)) == 1) {
        e.wr8(q + 0xc, 2); e.wr8(q + 0xb, 1);
    }
    const uint32_t r1 = e.h(0xE660, e.rd32(a + 0xc), 0, 0, e.rd32(a), 0, 0x20646b62);
    const uint32_t r2 = e.h(0xE660, e.rd32(a + 0xc), 0, 0, e.rd32(a), 0, 0x20736b62);
    if (r1) e.wr32(data + 0x7c, e.rd32(data + 0x7c) + 1);
    if (r2) e.wr32(data + 0x7c, e.rd32(data + 0x7c) + 1);
    if (r1) { e.wr8(q + 0xc, 5); e.wr8(q + 0xb, 3); e.wr8(data + 0x4e, 1); e.wr8(data + 0x78, 1); e.wr32(data + 0x58, 1); return 1; }
    if (r2) { e.wr8(data + 0x78, 1); e.wr8(q + 0xc, 4); e.wr8(q + 0xb, 2); e.wr32(data + 0x58, 1); return 1; }
    return r2;
}
}
