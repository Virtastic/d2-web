// 1.13c D2Game+0xD370 (a line of skill missiles laid across the caster's aim, centred on the target): ecx = pGame,
// edx = pUnit, stack = (skill id, skill level), ret 8. Written from the 1.13c disassembly as a rule over E (see d2game_c310.h).
// Rule: a skill id outside the table or a null record returns 0; the record's server missile (+0x48, signed) must lie in
// 1..dt+0xB6C exclusive. 0x9FA30 (eax = unit, ebx = &x, stack = &y; ret 4) gives the target's position; none returns 1.
// The offset (x - 0x1100(unit), y - 0x1130(unit)) is lengthened (0x9E490) and turned a quarter and shortened (0x9E450),
// both leaf helpers run here as retail runs them. n = 0xAC9A(unit, rec+0x138, id, level) missiles start n/2 steps back
// from the target; each is a 0x6F930 request (ecx = game, edx = 23-dword block: [0] = 0x20, [1] = [2] = unit, [4] = missile,
// [7], [8] = position, [11] = id, [12] = level, [19] = 0xAC9A(unit, rec+0x13C, id, level)), stepping by the offset.
// Then 0xA68E(unit, 0xC) nonzero: 0xACE8(unit, 8) and unit+0x48 = 0x600. Result 1.
#pragma once
#include <cstdint>

namespace d370 {
// 0x9E490 (esi = &dx, edx = &dy, plain ret): a short offset is scaled by 4, then by 2, until its squared length reaches 16
inline void Lengthen(int32_t& dx, int32_t& dy)
{
    uint32_t a = (uint32_t)dy * (uint32_t)dy + (uint32_t)dx * (uint32_t)dx;
    if ((int32_t)a < 4) { dx = (int32_t)((uint32_t)dx * 4); dy = (int32_t)((uint32_t)dy << 2); a = (uint32_t)dy * (uint32_t)dy + (uint32_t)dx * (uint32_t)dx; }
    if ((int32_t)a < 16) { dx = (int32_t)((uint32_t)dx + (uint32_t)dx); dy = (int32_t)((uint32_t)dy << 1); }
}
// 0x9E450 (ebx = &dx, edi = &dy, plain ret): (dx, dy) := (dy, -dx), both halved toward zero until the squared length is at most 3
inline void Turn(int32_t& dx, int32_t& dy)
{
    int32_t c = (int32_t)(0u - (uint32_t)dx), s = dy;
    while ((int32_t)((uint32_t)c * (uint32_t)c + (uint32_t)s * (uint32_t)s) > 3) { s /= 2; c /= 2; }
    dx = s; dy = c;
}
template <class E> uint32_t MissileLine(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t skillId, uint32_t level)
{
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skillId * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const int32_t m = e.rd16s(rec + 0x48);
    if (m <= 0 || m >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    uint32_t pos[2] = { 0, 0 };
    if (!e.h(0x9FA30, unit, 0, 0, e.stk(&pos[0]), 0, e.stk(&pos[1]))) return 1;
    int32_t dx = (int32_t)(pos[0] - e.h(0x1100, 0, unit));
    int32_t dy = (int32_t)(pos[1] - e.h(0x1130, 0, unit));
    Lengthen(dx, dy);
    Turn(dx, dy);
    int32_t n = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x138), (uint32_t)skillId, level);
    uint32_t x = pos[0] - (uint32_t)((int32_t)((uint32_t)n * (uint32_t)dx) / 2);
    uint32_t y = pos[1] - (uint32_t)((int32_t)((uint32_t)n * (uint32_t)dy) / 2);
    uint32_t q[23] = {};
    q[0] = 0x20; q[1] = unit; q[2] = unit; q[4] = (uint32_t)m; q[11] = (uint32_t)skillId; q[12] = level;
    q[19] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x13C), (uint32_t)skillId, level);
    for (; n > 0; n--) {
        q[7] = x; q[8] = y;
        e.h(0x6F930, 0, game, e.stk(q));
        x += (uint32_t)dx; y += (uint32_t)dy;
    }
    if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, 0xC)) {
        e.h(0xACE8, 0, 0, 0, 0, 0, unit, 8);
        e.wr32(unit + 0x48, 0x600);
    }
    return 1;
}
}
