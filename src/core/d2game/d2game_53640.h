// 1.13c D2Game+0x53640 (legacy save: the compact stat section, magic 0x6667 'gf'): EAX = section, EDI = unit; stack = (game, nStats, pSize), ret 0xC.
// Written from the 1.13c disassembly as a rule over E (HookEnv). *pSize := 0; a section whose first word is not 0x6667 returns 4.
// Stat i (0..nStats-1) is present when bit (i & 7) of byte section[2 + (i >> 3)] is set; the n-th present value is the dword at section + 5 + 4n,
// an absent stat is 0. Each stat goes through 0xA472 (Ordinal_10887 SetStat, stdcall unit, stat, value, 0). Then game+0x1D28 (null is the fatal
// path, never taken here): its byte +8 bit 2 sets stat 6 := 0xA3A0(unit) and stat 8 := 0xA4EA(unit); stat 10 := 0xA460(unit) always.
// *pSize := 4n + 5 and the result is 0.
#pragma once
#include <cstdint>

namespace g53640 {
template <class E> uint32_t ReadCompactStats(E& e, uint32_t sec, uint32_t unit, uint32_t game, int32_t nStats, uint32_t pSize)
{
    // 1.13c D2Game+0x53640
    e.wr32(pSize, 0);
    if ((e.rd32(sec) & 0xFFFF) != 0x6667) return 4;
    const uint32_t bits = sec + 2; uint32_t n = 0;
    for (int32_t i = 0; i < nStats; i++) {
        uint32_t v = 0;
        if (e.rd8(bits + ((uint32_t)i >> 3)) & (1u << (i & 7))) v = e.rd32(bits + 4 * n++ + 3);
        e.h(0xA472, 0, 0, 0, 0, 0, unit, (uint32_t)i, v, 0);
    }
    const uint32_t g = e.rd32(game + 0x1D28);
    if (!g) abort();   // retail: Ordinal_10024 fatal, then _exit
    if (e.rd8(g + 8) & 2) {
        e.h(0xA472, 0, 0, 0, 0, 0, unit, 6, e.h(0xA3A0, 0, 0, 0, 0, 0, unit), 0);
        e.h(0xA472, 0, 0, 0, 0, 0, unit, 8, e.h(0xA4EA, 0, 0, 0, 0, 0, unit), 0);
    }
    e.h(0xA472, 0, 0, 0, 0, 0, unit, 10, e.h(0xA460, 0, 0, 0, 0, 0, unit), 0);
    e.wr32(pSize, 4 * n + 5);
    return 0;
}
}
