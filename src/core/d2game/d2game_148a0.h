// 1.13c D2Game+0x148A0 (an even pick of a row from the 0x90-stride table that 0x144D0 weighs): six stack words (unit, needFlag54,
// bypassSeedBit, announce, secondRange, forcedRow), ret 0x18, a 0x1014-byte frame. Written from the 1.13c disassembly as a rule over E
// (see d2game_c310.h). Ghidra's "return address argument" is its misreading of the __chkstk frame.
// Rule: t = 0xB39C(); a null t returns 0. The range is [t+0xC, t+0x10) when secondRange is set, else [t+8, t+0xC); cnt = its length
// / 0x90 (signed); a null start returns cnt with the low word cleared; base = (start - t[1]) / 0x90. The unit's seed (0xA5EC) advances
// once; w = 0xB37E(unit). An even seed low word with bypassSeedBit clear returns 0. lv = 0xA8F8(unit) + 2, clamped to 1..99.
// cnt <= 0 returns cnt with the low word cleared. Row i is listed (index base + i, up to 0x1FF entries) when: needFlag54 wants
// byte +0x54; word +0x22 >= 100 wants the low word of w >= 100; +0x58 <= lv (signed); 0xB3C0(unit, row). No entry returns cnt with
// the low word cleared. Else roll = 0x1480(seed, n) (seed asked once more) picks an entry; a forcedRow with forcedRow + base > 0
// replaces it by the entry whose index is forcedRow + base - 1, or by index -1 when none is (the rolled row is still announced).
// announce sends 0xA9D0(0, 0, unit, row, 0, 0). Result index + 1.
#pragma once
#include <cstdint>

namespace d2g148a0 {
template <class E> uint32_t PickRow(E& e, uint32_t unit, uint32_t needFlag, uint32_t bypass, uint32_t announce, uint32_t second, int32_t forced)
{
    const uint32_t t = e.h(0xB39C);
    if (!t) return 0;
    const uint32_t lo = e.rd32(t + (second ? 0xC : 8)), hi = e.rd32(t + (second ? 0x10 : 0xC));
    const int32_t cnt = (int32_t)(hi - lo) / 0x90;
    if (!lo) return (uint32_t)cnt & 0xFFFF0000u;
    const int32_t base = (int32_t)(lo - e.rd32(t + 4)) / 0x90;
    uint32_t seed = e.h(0xA5EC, 0, 0, 0, 0, 0, unit);
    const uint64_t v = (uint64_t)e.rd32(seed) * 0x6AC690C5ull + e.rd32(seed + 4);
    e.wr32(seed, (uint32_t)v); e.wr32(seed + 4, (uint32_t)(v >> 32));
    const uint32_t w = e.h(0xB37E, 0, 0, 0, 0, 0, unit) & 0xFFFF;
    if (!(v & 1) && !bypass) return 0;
    int32_t lv = (int32_t)e.h(0xA8F8, 0, 0, 0, 0, 0, unit) + 2;
    lv = lv <= 1 ? 1 : lv < 99 ? lv : 99;
    if (cnt <= 0) return (uint32_t)cnt & 0xFFFF0000u;
    static uint32_t cand[0x1FF][2];
    int32_t n = 0;
    for (int32_t i = 0; i < cnt; i++) {
        const uint32_t row = lo + (uint32_t)i * 0x90u;
        if (needFlag && !e.rd8(row + 0x54)) continue;
        if ((uint32_t)(e.rd16s(row + 0x22) & 0xFFFF) >= 100 && w < 100) continue;
        if ((int32_t)e.rd32(row + 0x58) > lv) continue;
        if (!e.h(0xB3C0, 0, 0, 0, 0, 0, unit, row)) continue;
        if (n < 0x1FF) { cand[n][0] = (uint32_t)(i + base); cand[n][1] = row; n++; }
    }
    if (n <= 0) return (uint32_t)cnt & 0xFFFF0000u;
    seed = e.h(0xA5EC, 0, 0, 0, 0, 0, unit);
    uint32_t roll = 0;
    {
        const uint64_t v2 = (uint64_t)e.rd32(seed) * 0x6AC690C5ull + e.rd32(seed + 4);
        e.wr32(seed, (uint32_t)v2); e.wr32(seed + 4, (uint32_t)(v2 >> 32));
        roll = (uint32_t)v2 % (uint32_t)n;
    }
    uint32_t* ent = cand[roll];
    int32_t idx = (int32_t)ent[0];
    if (forced > 0 && forced + base > 0) {
        idx = -1;
        for (int32_t j = 0; j < n; j++)
            if ((int32_t)cand[j][0] == forced + base - 1) { idx = (int32_t)cand[j][0]; ent = cand[j]; break; }
    }
    if (announce) e.h(0xA9D0, 0, 0, 0, 0, 0, 0, 0, unit, ent[1]);
    return (uint32_t)(idx + 1);
}
}
