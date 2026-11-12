// 1.13c D2Game+0x22600: monster setup callback that fills a damage stat pair from the level table (table
// entry). eax = the entry value (returned for a null unit), ecx = the unit, edx = a word passed on, one stack
// word (ret 4). Only a monster (type 1) with monster data (+0x80) is handled; the other exits leave the type, 1
// or the clamped level in eax. The data's byte +0x6D, capped at 2, is the column; its byte +0x6A or word +0x74
// being set selects the second column group. The unit's level (0xA42A: unit, 12, 0), raised to at least 1 and
// capped at the level count less one (data tables +0xB74), picks a 0x78-byte record of the table at +0xB70,
// whose word 3 * (group + 6) + column is the base value. Two percentages come from 0x21FB0 (eax = column + 0x1C
// then + 0x1F with a nonzero stack word, + 0x10 then + 0x13 without); base * percentage / 100 (signed) are set
// as the stat pair (0xA4D2: unit, stat, value, 0), and 0x21BE0 finishes (ecx = unit, edx = the entry edx; the
// stack word), its result being returned.
#pragma once
#include <cstdint>

namespace g22600 {
template <class E> uint32_t Fill(E& e, uint32_t dt, uint32_t eax0, uint32_t unit, uint32_t edx0, uint32_t s0, uint32_t stat)
{
    if (!unit) return eax0;
    if (e.rd32(unit) != 1) return e.rd32(unit);
    const uint32_t md = e.rd32(unit + 0x80);
    if (!md) return 1;
    uint32_t col = e.rd8(md + 0x6D);
    if (col > 2) col = 2;
    const uint32_t grp = (e.rd8(md + 0x6A) || e.rd32(md + 0x74)) ? 1 : 0;
    int32_t lvl = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    const int32_t count = (int32_t)e.rd32(dt + 0xB74);
    if (lvl <= 1) lvl = 1;
    if (lvl >= count - 1) lvl = count - 1;
    if (lvl < 0 || lvl >= count) return (uint32_t)lvl;
    const uint32_t rec = e.rd32(dt + 0xB70) + (uint32_t)lvl * 0x78;
    if (!rec) return 0;
    const uint32_t base = e.rd32(rec + 4 * (col + 3 * (grp + 6)));
    const uint32_t k = s0 ? 0x1C : 0x10;
    const int32_t lo = (int32_t)(e.h(0x21FB0, col + k) * base) / 100;
    const int32_t hi = (int32_t)(e.h(0x21FB0, col + k + 3) * base) / 100;
    e.h(0xA4D2, 0, 0, 0, 0, 0, unit, stat, (uint32_t)lo, 0);
    e.h(0xA4D2, 0, 0, 0, 0, 0, unit, stat + 1, (uint32_t)hi, 0);
    return e.h(0x21BE0, 0, unit, edx0, 0, 0, s0);
}
}
