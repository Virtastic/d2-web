// 1.13c D2Game+0x50FE0, transcribed from the 1.13c disassembly: the Bone Prison / Bone Wall skill server do-function.
// Convention: ECX = game, EDX = unit, stack skill id, skill level (ret 8). dt is the data-tables block (sgptDataTables).
// 0x9FA30 (EAX = unit, EBX = &y; stack &x) finds the target cell; none returns 0. 0xA478 (D2Common 10331: unit) gives the room and
// 0x1280 (EAX = room, EBX = y, EDI = x) the room at the cell; none, or 0xA808 (D2Common 10057) on it nonzero, returns 0.
// The unit gets +0xC4 |= 0x40; the skill row (0x23C each at dt+0xB98, count dt+0xBA0, signed) or null; skill id 0 returns 0.
// 0x9EE50 (ECX = 0, EDX = &out; unit, 0, skill, level) below zero returns 0. The 9-dword request {9, unit, that result, 0, out, y, x,
// row byte +0xBE (0 when not below dt+0xBF0), 0xAC9A (D2Common 10786: unit, row +0xC0, skill, level)} goes to 0xA3610 (EAX = &request;
// game); a null object returns 0. A type-1 object stamps its path data (object +0x14, then +0x28): byte +8 |= 2, +0x2C unit id,
// +0x30 unit type (6 and -1 without a unit), +0x28 game. Then 0x232B0 (EBX = 0xF; object, 0), 0x4F970 (game, unit, object, skill,
// level, 0), 0xEFDB0 (EBX = 9; object, 0), the object's x and y (0x1100 / 0x1130, ECX = object); 0xA808 on the room again nonzero
// returns 0. half = 0xAC9A(unit, row +0x13C, skill, level) / 2 (signed); half <= 1, or the row word +0x48 negative or not below
// dt+0xB6C, returns 1. Otherwise two 23-dword requests to 0x6F930 (ECX = game, EDX = &request) at the cells offset by the
// perpendicular of the unit-to-object step, each result getting 0xA50E (D2Common 10637: it, object +0xC) and 0xAF5E (10019: it, half).
#pragma once
#include <cstdint>

namespace r50fe0 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    uint32_t x = 0, y = 0;
    if (!e.h(0x9FA30, unit, 0, 0, e.stk(&y), 0, e.stk(&x))) return 0;
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    e.ed(x);
    const uint32_t cell = e.h(0x1280, room, 0, 0, y);
    e.edi(0x1280, x);
    if (!cell || e.h(0xA808, 0, 0, 0, 0, 0, cell)) return 0;
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    const uint32_t rec = ((int32_t)skill >= 0 && (int32_t)skill < (int32_t)e.rd32(dt + 0xBA0)) ? e.rd32(dt + 0xB98) + skill * 0x23C : 0;
    if (!skill) return 0;
    uint32_t out[4] = { 0, 0, 0, 0 };
    const uint32_t r9 = e.h(0x9EE50, 0, 0, e.stk(out), 0, 0, unit, 0, skill, level);
    if ((int32_t)r9 < 0) return 0;
    uint32_t bt = e.rd8(rec + 0xBE);
    if ((int32_t)bt >= (int32_t)e.rd32(dt + 0xBF0)) bt = 0;
    uint32_t q[9] = { 9, unit, r9, 0, out[0], y, x, bt, 0 };
    q[8] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0xC0), skill, level);
    const uint32_t obj = e.h(0xA3610, e.stk(q), 0, 0, 0, 0, game);
    e.blk(0xA3610, q, 9);
    if (!obj) return 0;
    uint32_t typ = 6, id = 0xFFFFFFFFu;
    if (unit) { typ = e.rd32(unit); id = e.rd32(unit + 0xC); }
    if (e.rd32(obj) == 1) {
        const uint32_t pth = e.rd32(obj + 0x14);
        const uint32_t p = pth ? e.rd32(pth + 0x28) : 0;
        e.wr8(p + 8, e.rd8(p + 8) | 2);
        e.wr32(p + 0x2C, id);
        e.wr32(p + 0x30, typ);
        e.wr32(p + 0x28, game);
    }
    e.h(0x232B0, 0, 0, 0, 0xF, 0, obj, 0);
    e.h(0x4F970, 0, 0, 0, 0, 0, game, unit, obj, skill, level, 0);
    e.h(0xEFDB0, 0, 0, 0, 9, 0, obj, 0);
    const uint32_t ox = e.h(0x1100, 0, obj);
    const uint32_t oy = e.h(0x1130, 0, obj);
    if (e.h(0xA808, 0, 0, 0, 0, 0, cell)) return 0;
    const int32_t half = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x13C), skill, level) / 2;
    if (half <= 1) return 1;
    const int32_t w = e.rd16s(rec + 0x48);
    if (w < 0 || w >= (int32_t)e.rd32(dt + 0xB6C)) return 1;
    uint32_t m[23] = {};
    m[0] = 0x21; m[1] = unit; m[4] = (uint32_t)w; m[5] = ox; m[6] = oy; m[11] = skill; m[12] = level;
    uint32_t dx = e.h(0x1100, 0, unit) - ox;
    const uint32_t dy = e.h(0x1130, 0, unit) - oy;
    if (!dx && !dy) dx = 1;
    m[7] = ox - dy; m[8] = dx + oy;
    const uint32_t a = e.h(0x6F930, 0, game, e.stk(m));
    e.blk(0x6F930, m, 23);
    if (a) { e.h(0xA50E, 0, 0, 0, 0, 0, a, e.rd32(obj + 0xC)); e.h(0xAF5E, 0, 0, 0, 0, 0, a, (uint32_t)half); }
    m[7] = dy + ox; m[8] = oy - dx;
    const uint32_t b = e.h(0x6F930, 0, game, e.stk(m));
    e.blk(0x6F930, m, 23);
    if (b) { e.h(0xA50E, 0, 0, 0, 0, 0, b, e.rd32(obj + 0xC)); e.h(0xAF5E, 0, 0, 0, 0, 0, b, (uint32_t)half); }
    return 1;
}
}
