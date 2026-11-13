// 1.13c D2Game+0x59450 -- restores the quest state from the player flags, or starts the quest when the player carries its item
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f59450(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a), buf = e.qbuf(game, e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    uint32_t eax;
    eax = e.fl(buf, fid, 0);
    if (!eax) eax = e.fl(buf, fid, 0xf);
    if (!eax) { eax = e.fl(buf, fid, 1); if (eax != 1) eax = 0x80000000u; }
    if (eax != 0x80000000u) { e.wr8(q + 0xb, 5); return eax; }
    eax = e.fl(buf, fid, 4);
    if (eax == 1) { e.wr8(q + 0xb, 2); e.wr8(q + 0xc, 3); return eax; }
    eax = e.fl(buf, fid, 3);
    if (eax == 1) { e.wr8(q + 0xb, 1); e.wr8(q + 0xc, 3); return eax; }
    eax = e.fl(buf, fid, 2);
    if (eax == 1) { e.wr8(q + 0xb, 1); e.wr8(q + 0xc, 2); return eax; }
    eax = e.h(0x69590, e.rd32(a + 0xc), 0, 0, 0, 0x201, game);
    if (!eax) return eax;
    eax = NE::lo(eax, e.rd8(q + 0xc)); if (eax & 0xff) return eax;
    eax = NE::lo(eax, e.rd8(q + 9)); if (!(eax & 0xff)) return eax;
    e.wr8(q + 0xc, 1);
    return eax;
}
}
