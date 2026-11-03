// 1.13c D2Game+0x6BF00 (spawn two type-2 world objects beside a unit): ecx = game, edx = p2 (a unit), plain ret, no result.
// Written from the 1.13c disassembly as a rule over an environment E: every callee that is not a field read is one E::h(rva, ...) call.
// pos = 0xA544(p2, &pos) and room = 0xA478(p2) are read once. Spawn A (class 0xA2) at (pos.x, pos.y) in that room: unit = 0xA6B8(game+0x1C, 2); a null
// unit skips the whole spawn. Then 0xA6D0(room, &rect): if rect[0] <= pos.x+1 < rect[0]+rect[2] (signed) a second spawn B (class 0xA0) at (pos.x+1, pos.y)
// in room = 0xA478(p2) again; a null unit there is the fatal 0xA48 (not driven). The spawn body is SUnit's inlined allocation as in OBJECTS_SpawnShrine:
// header words, +0xC8 flag 0x2000000 from game+0x70 then 0x4000000, act byte (0xA556 then 0xA56E, low byte) with game+0xBC+4*act, 0xE0110 (edi unit,
// esi game+0xD0), +0xC4 |= 0x10, +0xD0 = 0xB, the game+0x98 id counter (wraps 0xFFFFFFFF to 1), a 0x38 data block (0xA2E6) zeroed with the txt row
// (0xA520) in word 0, +0x10 = 1, 0xA724, 0x2F300(esi unit; id, room, x, y), 0xE0F40(eax room, ecx y, edx unit; x, game, 1), +0x78 = 2.
#pragma once
#include <cstdint>

namespace r6bf00 {
template <class E> uint32_t Spawn(E& e, uint32_t game, uint32_t cls, uint32_t room, uint32_t x, uint32_t y)
{
    const uint32_t pool = e.rd32(game + 0x1C);
    const uint32_t u = e.h(0xA6B8, 0, 0, 0, 0, 0, pool, 2);
    if (!u) return 0;
    e.wr32(u + 0, 2); e.wr32(u + 4, cls); e.wr32(u + 8, e.rd32(game + 0x1C)); e.wr32(u + 0x80, game);
    uint32_t fl = e.rd32(u + 0xC8);
    fl = e.rd32(game + 0x70) ? (fl | 0x2000000u) : (fl & 0xFDFFFFFFu);
    e.wr32(u + 0xC8, fl); e.wr32(u + 0xC8, fl | 0x4000000u);
    const uint32_t act = e.h(0xA56E, 0, 0, 0, 0, 0, e.h(0xA556, 0, 0, 0, 0, 0, room)) & 0xFF;
    e.wr8(u + 0x18, act);
    e.wr32(u + 0x1C, e.rd32(game + 0xBC + act * 4));
    e.wr32(u + 0x74, 0);
    e.ed(u); e.h(0xE0110, 0, 0, 0, 0, game + 0xD0); e.edi(0xE0110, u);
    e.wr32(u + 0xC4, e.rd32(u + 0xC4) | 0x10);
    e.wr32(u + 0xD0, 0xB);
    uint32_t id = e.rd32(game + 0x98) + 1;
    if (id == 0xFFFFFFFFu) id = 1;
    e.wr32(game + 0x98, id);
    const uint32_t ud = e.h(0xA2E6, 0, e.rd32(game + 0x1C), 0x38, 0, 0, 0xB20FB02Cu, 0x265, 0);
    e.wr32(u + 0x14, ud);
    for (uint32_t i = 0; i < 14; i++) e.wr32(ud + 4 * i, 0);
    e.wr32(e.rd32(u + 0x14), e.h(0xA520, 0, 0, 0, 0, 0, cls));
    e.wr32(u + 0x10, 1);
    e.h(0xA724, 0, 0, 0, 0, 0, u);
    e.h(0x2F300, 0, 0, 0, 0, u, id, room, x, y);
    e.h(0xE0F40, room, y, u, 0, 0, x, game, 1);
    e.wr8(u + 0x78, 2);
    return u;
}

template <class E> void Run(E& e, uint32_t game, uint32_t p2)
{
    uint32_t pos[2] = {0, 0};
    e.h(0xA544, 0, 0, 0, 0, 0, p2, e.stk(pos));
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, p2);
    const uint32_t x = pos[0], y = pos[1];
    Spawn(e, game, 0xA2, room, x, y);
    uint32_t rect[4] = {0, 0, 0, 0};
    e.h(0xA6D0, 0, 0, 0, 0, 0, room, e.stk(rect));
    const int32_t x1 = (int32_t)(x + 1);
    if (x1 < (int32_t)rect[0] || x1 >= (int32_t)(rect[2] + rect[0])) return;
    const uint32_t room2 = e.h(0xA478, 0, 0, 0, 0, 0, p2);
    if (!Spawn(e, game, 0xA0, room2, (uint32_t)x1, y)) __builtin_trap();   // retail: fatal 0xA48
}
}
