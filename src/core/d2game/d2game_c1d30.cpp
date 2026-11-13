// 1.13c D2Game+0xC1D30 -- npc menu state byte of quest 0xE for a player that finished quest 7
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fc1d30(NE& e, uint32_t q, uint32_t d, uint32_t s0, uint32_t buf, uint32_t out)
{
    uint32_t r = e.fl(buf, 7, 0);
    if (!r) { e.wr8(out, 0); return NE::lo(out, 1); }
    const uint32_t data = e.rd32(q + 0x18);
    if (e.rd8(data + 0x10)) { e.wr8(out, e.rd8(data + 0x14)); return NE::lo(data, 1); }
    e.wr8(out, 0);
    r = e.fl(buf, 0xe, 0);
    if (r) return NE::lo(r, 1);
    r = e.fl(buf, 0xe, 3);
    if (r == 1) { e.wr8(out, 5); return r; }
    r = e.fl(buf, 0xe, 4);
    if (r == 1) { e.wr8(out, 6); return r; }
    r = NE::lo(r, e.rd8(q + 9));
    if (!e.rd8(q + 9)) return NE::lo(r, 1);
    r = e.fl(buf, 0xe, 5);
    if (r == 1) { e.wr8(out, e.rd8(q + 0xb)); return 1; }
    if (e.rd8(q + 0xc) < 3) e.wr8(out, e.rd8(q + 0xb));
    else e.wr8(out, 0xc);
    return NE::lo(r, 1);
}
}
