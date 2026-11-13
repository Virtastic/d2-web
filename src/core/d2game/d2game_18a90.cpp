// 1.13c D2Game+0x18A90: a monster AI step that picks a mode change, an attack skill or a move at random. ECX = game, EDX = unit, one stack word (ret 4) = a pointer to the
// think record: +0x08 the target unit, +0x14 a distance, +0x18 a flag, +0x1C the monster row (int16 percentages at row[+0x56 + 2 * byte game[0x6D]] and so on).
// Every roll is the unit seed (dwords +0x20 / +0x24, step lo*0x6AC690C5 + hi as 64 bits) new low word % 100, compared signed below a row percentage. The unit's flag word +0xC4 is
// or-ed with 0xE first. With the think flag set: roll below row[+0x56] sends a mode change (0xA496(unit, 0) first, then 0x75E10(game, &block, 1), the block eight dwords {4, unit,
// target, 0, 0, byte-1 value 0x64 (0x65 when 0x1320(EAX unit class) has a record whose byte +0x104 meets the +0x10 mask of the table *0xF8BE0) , 0, 0}); otherwise 0xF22C0(EAX game,
// ESI unit; int16 row[+0x5C + 2 * byte]). Without the flag: when int16 row[+0x172] >= 0, 0xB0E90(ECX unit) is nonzero, the distance is below int16 row[+0x62 + ..] and above 5 and
// 0x1180(EAX 100, ECX the seed address; the table's own roll, not the unit seed) is below int16 row[+0x68 + ..]: 0xF2380(EAX = skill | byte row[+0x181], ECX unit, EBX game; skill
// int16 row[+0x172], target, 0, 0) and done. Else a seed roll below row[+0x6E + ..] runs 0xF2540(ECX game, EDX unit; target, 2, 0, 0, 1, 0); else a second seed roll below row[+0x74 + ..]
// runs 0xF1D90(EAX 4, ECX target, EDX unit; game, unit, 2, 0); else 0xF22C0(EAX game, ESI unit; int16 row[+0x7A + ..]).
#include "d2game_seam4e.h"
static uint32_t step(uint32_t seed)
{
    const uint64_t v = (uint64_t)d2rd32(seed) * 0x6AC690C5ull + d2rd32(seed + 4);
    d2wr32(seed + 4, (uint32_t)(v >> 32));
    d2wr32(seed, (uint32_t)v);
    return (uint32_t)v % 100u;
}
extern "C" void d2g_18a90(uint32_t game, uint32_t unit, uint32_t arg, uint32_t tbl, const D2Seam* e)
{
    const uint32_t dist = d2rd32(arg + 0x14), flag = d2rd32(arg + 0x18), row = d2rd32(arg + 0x1C), target = d2rd32(arg + 0x8);
    const uint32_t seed = unit + 0x20;
    if (unit) d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0xE);
    const uint32_t gb = d2rd8(game + 0x6D);
    auto pct = [&](uint32_t off) { return (int32_t)(int16_t)d2rd16(row + gb * 2 + off); };
    if (flag) {
        const uint32_t roll = step(seed);
        if ((int32_t)roll >= pct(0x56)) {
            e->call(0xF22C0, game, 0, 0, 0, unit, (uint32_t)pct(0x5C));
            return;
        }
        e->call(0xA496, 0, 0, 0, 0, 0, unit, 0);
        uint32_t blk[8] = {};
        blk[0] = 4;
        blk[1] = unit;
        blk[2] = target;
        uint32_t v = 0x64;
        if (unit) {
            const uint32_t rec = e->call(0x1320, 0);
            if (rec && (d2rd8(rec + 0x104) & d2rd32(tbl + 0x10))) v = 0x65;
        }
        blk[5] = v << 8;
        const uint32_t bp = (uint32_t)(uintptr_t)blk;
        e->call(0x75E10, 0, 0, 0, 0, 0, game, bp, 1);
        e->blk(0x75E10, blk, 8);
        return;
    }
    if ((int16_t)d2rd16(row + 0x172) >= 0 && e->call(0xB0E90, 0, unit) && (int32_t)dist < pct(0x62) && (int32_t)dist > 5 &&
        (int32_t)e->call(0x1180, 0, seed) < pct(0x68)) {
        const uint32_t skill = (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x172);
        e->call(0xF2380, (skill & ~0xFFu) | d2rd8(row + 0x181), unit, 0, game, 0, skill, target, 0, 0);
        return;
    }
    if ((int32_t)step(seed) < pct(0x6E)) {
        e->call(0xF2540, 0, game, unit, 0, 0, target, 2, 0, 0, 1, 0);
        return;
    }
    if ((int32_t)step(seed) < pct(0x74)) {
        e->call(0xF1D90, 4, target, unit, 0, 0, game, unit, 2, 0);
        return;
    }
    e->call(0xF22C0, game, 0, 0, 0, unit, (uint32_t)pct(0x7A));
}
