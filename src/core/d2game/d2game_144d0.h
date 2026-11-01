// 1.13c D2Game+0x144D0 (a weighted pick of a row from a 0x90-stride table for the unit's level): seven stack words
// (unit, needFlag54, bypassSeedBit, announce, secondRange, forcedRow, idFilter), ret 0x1C, a 0x1024-byte frame. Written from the
// 1.13c disassembly as a rule over E (see d2game_c310.h). Ghidra's "return address argument" is its misreading of the __chkstk frame.
// Rule: t = 0xB39C(); a null t returns 0. The range is [t+0x10, t[0] * 0x90 + t[1]) when idFilter is set, else [t+0xC, t+0x10) when
// secondRange is set, else [t+8, t+0xC); cnt = its length / 0x90 (signed), a null start returns cnt with the low word cleared;
// base = (start - t[1]) / 0x90. The unit's seed (0xA5EC) advances once; w = 0xB37E(unit), ul = byte 0xB390(unit). An even seed
// low word with bypassSeedBit clear returns 0. lv = max(0xA8F8(unit), ul) (signed); m = 0xA53E(unit ? unit+4 : -1), null returns 0;
// f = byte m+0x140: lv += f, or without f, h = ul / 2, c = 99 - h: lv - h below c, else 2 * lv - h - c; lv is clamped to 1..99.
// cnt <= 0 returns cnt with the low word cleared. Each row i: needFlag54 wants byte +0x54; word +0x22 >= 100 wants the low word
// of w >= 100; forcedRow - 1 == i (forcedRow > 0) skips the level test, else +0x58 > lv or a nonzero +0x60 below lv skips; byte
// +0x64 clear asks 0xA898(unit), 6, 8 and 9 skipping; 0xB3C0(unit, row) must hold; idFilter must equal +0x5C; byte +0x82 must be
// nonzero; byte +0x66 other than 0xFF must equal 0xB3D8(unit) unless that is 7 (asked once more for the compare); 0x131E0 (ebx =
// +0x5C, edi = unit; plain ret) must be 0. The forced row is the pick (index base + i, when not negative); others go to a 0x1FF-entry
// list and the weight sum (byte +0x82, times +0x58 with f) counts every one. No candidate returns the last loop eax with the low
// word cleared. Else roll = 0x1480(seed, sum + 1) (seed asked once more) and the list is walked subtracting weights until the
// running value goes negative; that entry (or the last) is the pick. announce sends 0xA9D0(0, 0, unit, row, 0, 0). Result index + 1.
#pragma once
#include <cstdint>

namespace d2g144d0 {
template <class E> uint32_t Roll(E& e, uint32_t seed, int32_t n)
{
    if (n <= 0) return 0;
    const uint64_t v = (uint64_t)e.rd32(seed) * 0x6AC690C5ull + e.rd32(seed + 4);
    e.wr32(seed, (uint32_t)v); e.wr32(seed + 4, (uint32_t)(v >> 32));
    return (uint32_t)v % (uint32_t)n;
}
template <class E> uint32_t PickRow(E& e, uint32_t unit, uint32_t needFlag, uint32_t bypass, uint32_t announce, uint32_t second,
                                    int32_t forced, uint32_t idFilter)
{
    const uint32_t t = e.h(0xB39C);
    if (!t) return 0;
    uint32_t lo, hi;
    if (idFilter) { lo = e.rd32(t + 0x10); hi = e.rd32(t) * 0x90u + e.rd32(t + 4); }
    else if (second) { lo = e.rd32(t + 0xC); hi = e.rd32(t + 0x10); }
    else { lo = e.rd32(t + 8); hi = e.rd32(t + 0xC); }
    const int32_t cnt = (int32_t)(hi - lo) / 0x90;
    if (!lo) return (uint32_t)cnt & 0xFFFF0000u;
    const int32_t base = (int32_t)(lo - e.rd32(t + 4)) / 0x90;
    uint32_t seed = e.h(0xA5EC, 0, 0, 0, 0, 0, unit);
    const uint64_t v = (uint64_t)e.rd32(seed) * 0x6AC690C5ull + e.rd32(seed + 4);
    e.wr32(seed, (uint32_t)v); e.wr32(seed + 4, (uint32_t)(v >> 32));
    const uint32_t w = e.h(0xB37E, 0, 0, 0, 0, 0, unit) & 0xFFFF;
    const int32_t ul = (int32_t)(e.h(0xB390, 0, 0, 0, 0, 0, unit) & 0xFF);
    if (!(v & 1) && !bypass) return 0;
    int32_t lv = (int32_t)e.h(0xA8F8, 0, 0, 0, 0, 0, unit);
    const uint32_t m = e.h(0xA53E, 0, 0, 0, 0, 0, unit ? e.rd32(unit + 4) : 0xFFFFFFFFu);
    if (!m) return 0;
    if (lv <= ul) lv = ul;
    const int32_t f = (int32_t)e.rd8(m + 0x140);
    if (f) lv += f;
    else {
        const int32_t h = ul / 2, c = 99 - h;
        lv = lv < c ? lv - h : 2 * lv - h - c;
    }
    lv = lv <= 1 ? 1 : lv < 99 ? lv : 99;
    if (cnt <= 0) return (uint32_t)cnt & 0xFFFF0000u;
    static uint32_t cand[0x1FF][2];
    int32_t n = 0;
    uint32_t sum = 0, eaxLeft = (uint32_t)cnt;
    int32_t idx = -1; uint32_t pick = 0; bool have = false;
    for (int32_t i = 0; i < cnt; i++) {
        const uint32_t row = lo + (uint32_t)i * 0x90u;
        if (needFlag && !e.rd8(row + 0x54)) continue;
        if ((uint32_t)(e.rd16s(row + 0x22) & 0xFFFF) >= 100 && w < 100) continue;
        const bool isForced = forced > 0 && forced - 1 == i;
        if (!isForced) {
            if ((int32_t)e.rd32(row + 0x58) > lv) continue;
            const int32_t mx = (int32_t)e.rd32(row + 0x60);
            if (mx && mx < lv) continue;
        }
        if (!e.rd8(row + 0x64)) {
            const int32_t k = (int32_t)e.h(0xA898, 0, 0, 0, 0, 0, unit);
            if (k == 6 || (k > 7 && k <= 9)) continue;
        }
        if (!e.h(0xB3C0, 0, 0, 0, 0, 0, unit, row)) continue;
        if (idFilter && idFilter != e.rd32(row + 0x5C)) continue;
        if (!e.rd8(row + 0x82)) continue;
        if (e.rd8(row + 0x66) != 0xFF && e.h(0xB3D8, 0, 0, 0, 0, 0, unit) != 7 && e.rd8(row + 0x66) != e.h(0xB3D8, 0, 0, 0, 0, 0, unit)) continue;
        if (e.h(0x131E0, 0, 0, 0, e.rd32(row + 0x5C))) continue;
        if (isForced) {
            if (i + base >= 0) { idx = i + base; pick = row; have = true; }
            else eaxLeft = row;
            break;
        }
        if (n < 0x1FF) { cand[n][0] = (uint32_t)(i + base); cand[n][1] = row; n++; }
        sum += f ? e.rd8(row + 0x82) * e.rd32(row + 0x58) : e.rd8(row + 0x82);
    }
    if (!have) {
        if (n <= 0) return eaxLeft & 0xFFFF0000u;
        seed = e.h(0xA5EC, 0, 0, 0, 0, 0, unit);
        int32_t left = (int32_t)Roll(e, seed, (int32_t)(sum + 1));
        int32_t j = 0;
        for (; j < n; j++) {
            const uint32_t row = cand[j][1];
            left -= (int32_t)(f ? e.rd8(row + 0x82) * e.rd32(row + 0x58) : e.rd8(row + 0x82));
            if (left < 0) break;
        }
        if (j == n) j = n - 1;
        idx = (int32_t)cand[j][0]; pick = cand[j][1];
    }
    if (announce) e.h(0xA9D0, 0, 0, 0, 0, 0, 0, 0, unit, pick);
    return (uint32_t)(idx + 1);
}
}
