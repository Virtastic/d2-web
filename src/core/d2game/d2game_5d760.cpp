// 1.13c D2Game+0x5D760 -- spawns the object 0x235 five columns right of a unit standing in a level 0x84 room
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5d760(NE& e, uint32_t g, uint32_t unit, uint32_t s0, uint32_t x, uint32_t y, uint32_t outv)
{
    const uint32_t room = e.imp(0xA478, unit);
    if (!room) return 0;
    if (e.imp(0xA556, room) != 0x84) return 0;
    uint32_t pos[2] = { x, y };                        // D2Common 10141 fills the pair
    e.imp(0xA544, unit, e.stk(pos));
    pos[0] += 5;
    uint32_t out = e.imp(0xA478, unit);
    e.h(0x34130, 0, 0, 0, 0, 0, room, e.stk(pos), 5, 0x400, e.stk(&out), 0x64);
    out = outv;                                        // 0x34130 answers the room through its fifth word
    if (!out) return 1;
    const uint32_t o = e.imp(0xA6B8, e.rd32(g + 0x1c), 2);
    if (!o) return 1;
    e.wr32(o, 2); e.wr32(o + 4, 0x235); e.wr32(o + 8, e.rd32(g + 0x1c)); e.wr32(o + 0x80, g);
    uint32_t f = e.rd32(o + 0xc8);
    f = e.rd32(g + 0x70) ? f | 0x2000000u : f & 0xfdffffffu;
    e.wr32(o + 0xc8, f | 0x4000000u);
    const uint32_t lv = e.imp(0xA56E, e.imp(0xA556, out));
    e.wr8(o + 0x18, lv); e.wr32(o + 0x1c, e.rd32(g + 0xbc + 4 * (lv & 0xff))); e.wr32(o + 0x74, 0);
    e.h(0xE0110, 0, 0, 0, 0, g + 0xd0); e.edi(0xE0110, o);
    e.wr32(o + 0xc4, e.rd32(o + 0xc4) | 0x10); e.wr32(o + 0xd0, 0xb);
    uint32_t id = e.rd32(g + 0x98) + 1;
    if (id == 0xffffffffu) id = 1;
    e.wr32(g + 0x98, id);
    const uint32_t m = e.h(0xA2E6, 0, e.rd32(g + 0x1c), 0x38, 0, 0, 0xB2000000u | 0xFB02C, 0x265, 0);
    e.wr32(o + 0x14, m);
    for (uint32_t i = 0; i < 0x38; i += 4) e.wr32(m + i, 0);
    e.wr32(m, e.imp(0xA520, 0x235));
    e.wr32(o + 0x10, 1);
    e.imp(0xA724, o);
    e.h(0x2F300, 0, 0, 0, 0, o, id, out, pos[0], pos[1]);
    e.h(0xE0F40, out, pos[1], o, 0, 0, pos[0], g, 1);
    return 1;
}
}
