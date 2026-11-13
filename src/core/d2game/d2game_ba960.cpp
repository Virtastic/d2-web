// 1.13c D2Game+0xBA960 -- quest 0x1B tick: a state 4 quest moves to phase 3, and a pending spawn places monster 0x196 near the stored spot
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fba960(NE& e, uint32_t eax0, uint32_t game, uint32_t q)
{
    const uint32_t data = e.rd32(q + 0x18);
    uint32_t eax = eax0;
    if (e.rd8(q + 0xc) == 4) {
        const uint32_t b = e.rd8(q + 0xb);
        eax = NE::lo(eax, b);
        if (b != 4 && b != 0xd) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 3); eax = e.cb(e.rd32(q + 4), 0xB9D20); }
    }
    eax = NE::lo(eax, e.rd8(data + 0x10));
    if (e.rd8(data + 0x10)) {
        e.wr8(data + 0x10, 0);
        const uint32_t x = e.rd32(data + 4), y = e.rd32(data + 8);
        const uint32_t room = eax = e.imp(0xA526, e.rd32(game + 0xc8), x, y);
        if (room) {
            eax = e.h(0xEF650, 0, 0x196, 8, 0, 0, game, room, x, y, 0xffffffffu, 0);
            if (!eax) eax = e.h(0xEF650, 0, 0x196, 8, 0, 0, game, room, x, y, 3, 0);
            if (!eax) eax = e.h(0xEF650, 0, 0x196, 8, 0, 0, game, room, x, y, 5, 0);
        }
    }
    e.wr8(data + 0xd, 0);
    return NE::lo(eax, 1);
}
}
