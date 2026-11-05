// D2Game 0xF29B0: walk a unit to a random offset around itself. EAX low byte = range, stack game, unit (ret 8).
// Position: type 2 or 4..5 reads the path record at +0x2C (+0xC x, +0x10 y); other types call 0xA454 / 0xA3B8 on it
// (0 when the pointer is null). The unit's seed (+0x20, 64-bit, step lo*0x6AC690C5 + hi) steps three times: the first
// parity picks which of (range roll from 0x11D0, range) becomes dx and which dy; the second and third negate dx and dy.
// The walk is 0xF2540 (ECX game, EDX unit; 0, 2, x, y, 1, 0).
#include "d2game_s15.h"

static uint32_t step(uint32_t seed) {
    const uint64_t v = (uint64_t)d2rd32(seed) * 0x6AC690C5u + d2rd32(seed + 4);
    d2wr32(seed, (uint32_t)v); d2wr32(seed + 4, (uint32_t)(v >> 32));
    return (uint32_t)v;
}
// 1.13c D2Game+0xF29B0
extern "C" uint32_t d2game_f29b0(uint32_t eax, uint32_t game, uint32_t unit, const D2S15* e)
{
    const D2Seam& s = *e->sm;
    const uint32_t al = eax & 0xFF, t = d2rd32(unit), path = d2rd32(unit + 0x2C);
    const bool st = t == 2 || (t > 3 && t <= 5) ? true : false;
    // retail's compare is signed: <= 3 (and != 2) is the dynamic arm, > 5 is dynamic
    uint32_t x, y;
    if (st) x = d2rd32(path + 0xC); else x = path ? s.call(0xA454, 0, 0, 0, 0, 0, path) : 0;
    if (st) y = d2rd32(path + 0x10); else y = path ? s.call(0xA3B8, 0, 0, 0, 0, 0, path) : 0;
    const uint32_t seed = unit + 0x20;
    uint32_t dx, dy;
    const uint32_t p1 = step(seed);
    const uint32_t r = s.call(0x11D0, 0, seed, 0, 0, al);
    if (p1 & 1) { dy = r; dx = al; } else { dx = r; dy = al; }
    if (step(seed) & 1) dx = 0u - dx;
    if (step(seed) & 1) dy = 0u - dy;
    const uint32_t w[6] = { 0, 2, dx + x, dy + y, 1, 0 };
    const uint32_t ret = s.call(0xF2540, 0, game, unit, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5]);
    s.blk(0xF2540, w, 6);
    return ret;
}
