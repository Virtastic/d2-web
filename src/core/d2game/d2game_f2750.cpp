// D2Game+0xF2750 -- AITACTICS_RunCloseToTargetUnit: ECX = target unit, AL = max distance, stack game, stack unit (ret 8).
// Written from the 1.13c disassembly. The target position is the path record's x/y (types 2, 4, 5) or the 0xA454/0xA3B8 thunks.
// The unit seed (+0x20 lo, +0x24 hi) steps inline (x * 0x6AC690C5 + hi); the limited roll is 0x11D0 (ECX = seed, ESI = limit);
// the move is 0xF2540 (ECX = game, EDX = unit, stack 0, 0xF, x, y, 1, 0). Callees are the D2Seam.
#include "d2game_seam4e.h"
static uint32_t f2750_step(uint32_t seed)
{
    const uint64_t p = (uint64_t)d2rd32(seed) * 0x6AC690C5ull + d2rd32(seed + 4);
    d2wr32(seed + 4, (uint32_t)(p >> 32));
    d2wr32(seed, (uint32_t)p);
    return (uint32_t)p;
}
// 1.13c D2Game+0xF2750
extern "C" uint32_t d2game_f2750(uint32_t tgt, uint32_t al, uint32_t game, uint32_t unit, const D2Seam* e)
{
    const int32_t type = (int32_t)d2rd32(tgt);
    const bool direct = type == 2 || (type > 3 && type < 6);
    uint32_t tx, ty;
    if (direct) { tx = d2rd32(d2rd32(tgt + 0x2C) + 0xC); ty = d2rd32(d2rd32(tgt + 0x2C) + 0x10); }
    else if (!d2rd32(tgt + 0x2C)) { tx = 0; ty = 0; }
    else { tx = e->call(0xA454, 0, 0, 0, 0, 0, d2rd32(tgt + 0x2C)); ty = e->call(0xA3B8, 0, 0, 0, 0, 0, d2rd32(tgt + 0x2C)); }
    const uint32_t seed = unit + 0x20, max = al & 0xFF;
    uint32_t sx, sy;
    if (f2750_step(seed) & 1) { sx = max; sy = e->call(0x11D0, 0, seed, 0, 0, max); }
    else { sy = max; sx = e->call(0x11D0, 0, seed, 0, 0, max); }
    if (f2750_step(seed) & 1) sx = 0u - sx;
    if (f2750_step(seed) & 1) sy = 0u - sy;
    return e->call(0xF2540, 0, game, unit, 0, 0, 0, 0xF, sx + tx, sy + ty, 1, 0);
}
