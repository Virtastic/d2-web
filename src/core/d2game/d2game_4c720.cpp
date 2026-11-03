// 1.13c D2Game+0x4C720 skill srv: ecx=game, edx=unit, stack skill,lvl (ret 8); fills a zeroed 0x70-byte stack block for 0x9F210.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4c720(uint32_t game, uint32_t unit, uint32_t skill, uint32_t lvl, uint32_t tab, const D2Seam* e)
{
    e->call(0xE07A0, unit, game);
    const uint32_t p = e->call(0xA598,0,0,0,0,0,d2rd32(unit+0x2c));
    if (p == unit || p == 0) return 0;
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)d2rd32(tab + 0xba0)) return 0;
    const uint32_t row = skill * 0x23c + d2rd32(tab + 0xb98);
    if (!row) return 0;
    uint32_t st[28] = {};
    const uint32_t ax = e->call(0xDE5A0, p, 0,0,0,0, game, 0, 0) & 0xFFFF;
    st[1] = ax;
    if (ax & 1) {
        st[3] = e->call(0xAC9A,0,0,0,0,0,unit,d2rd32(row+0x138),skill,lvl);
        if ((uint8_t)d2rd8(row+0x1dc)) {
            const uint32_t t = e->call(0xAC9A,0,0,0,0,0,unit,d2rd32(row+0x144),skill,lvl);
            st[26] = t;
            if ((int32_t)t > 0) ((uint8_t*)st)[0x65] = (uint8_t)d2rd8(row+0x1dc);
        }
        e->call(0x9F210, skill, 0,0,0,0, (uint32_t)(uintptr_t)st, lvl);
        e->blk(0x9F210, st, 28);
    }
    e->call(0xDDDE0, unit, (game & ~0xffu) | d2rd8(row+0x1a5), 0,0,0, game, p);
    return 1;
}
