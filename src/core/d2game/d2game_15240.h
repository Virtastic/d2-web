// 1.13c D2Game+0x15240 (object roll of its two lists of 3 slot values): ESI = unit, stack = p (ret 4); returns 0 or 1.
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). The frame keeps EBX, EBP and EDI only (ESI is the caller's unit),
// so the word at [esp+0x30] the body reads is the stack argument p, not the return address.
// x = (0xB37E(u) & 0xFFFF) >= 1 ? 0x14210(u, 1) : 0x14370(u, 1); y the same with 0. Either low word 0 returns 0. 0xB0AE(u, x), 0xB0A8(u, y).
// seed = 0xA5EC(u); class 0xAA66(u) == 0x3A gives n = (step & 1) + 3, else n = table[step & 7] (module global 0xFD058, 8 dwords); n <= 0 returns 0.
// Up to n rounds, A = list p +0x68, B = list p +0x74, each filled 0..3 through 0x144D0(u, 1, 1, 0, which, list[k], 0), which = 1 for A, 0 for B:
// with A still open and B open the round picks B on an odd step, else A; with only one open that one; both closed ends the rounds. A zero low
// word closes that list and does not count the round; a nonzero writes 0xB3C6(u, v, kA) for A or 0xB3B4(u, v, kB) for B, and its third
// value closes it. kA = kB = 0 returns 0. Then 0xA65E(u, 0x10, 0); for i 0..2: s = 0xB3AE(0xA9C4(u, i) & 0xFFFF), s set runs
// 0xA9D0(0, 0, u, s, 0, 0); the same with 0xA90A. 0x13DE0 (EAX = u, stack = p). Returns 1. A list index at 3 (never reached) is retail's fatal exit.
#pragma once
#include <cstdint>

namespace g15240 {
template <class E> uint32_t Step(E& e, uint32_t seed)
{
    const uint64_t n = (uint64_t)e.rd32(seed) * 0x6AC690C5u + e.rd32(seed + 4);
    e.wr32(seed, (uint32_t)n); e.wr32(seed + 4, (uint32_t)(n >> 32));
    return (uint32_t)n;
}
template <class E> uint32_t RollSlots(E& e, uint32_t u, uint32_t p, uint32_t table)
{
    // 1.13c D2Game+0x15240
    auto pick = [&](uint32_t w) {
        return (e.h(0xB37E, 0, 0, 0, 0, 0, u) & 0xFFFF) >= 1 ? e.h(0x14210, 0, 0, 0, 0, 0, u, w) : e.h(0x14370, 0, 0, 0, 0, 0, u, w);
    };
    const uint32_t x = pick(1), y = pick(0);
    if (!(x & 0xFFFF) || !(y & 0xFFFF)) return 0;
    e.h(0xB0AE, 0, 0, 0, 0, 0, u, x);
    e.h(0xB0A8, 0, 0, 0, 0, 0, u, y);
    const uint32_t seed = e.h(0xA5EC, 0, 0, 0, 0, 0, u);
    const int32_t n = e.h(0xAA66, 0, 0, 0, 0, 0, u) == 0x3A ? (int32_t)((Step(e, seed) & 1) + 3) : (int32_t)e.rd32(table + 4 * (Step(e, seed) & 7));
    if (n <= 0) return 0;
    uint32_t kA = 0, kB = 0; bool doneA = false, doneB = false;
    for (int32_t i = 0; i < n; i++) {
        bool useB;
        if (doneA) { if (doneB) break; useB = true; }
        else if (!doneB) useB = Step(e, seed) & 1;
        else useB = false;
        const uint32_t k = useB ? kB : kA, item = e.rd32(p + (useB ? 0x74 : 0x68) + 4 * k);
        const uint32_t v = e.h(0x144D0, useB ? 0 : 1, item, 0, 0, 0, u, 1, 1, 0, useB ? 0 : 1, item, 0);
        if (!(v & 0xFFFF)) { (useB ? doneB : doneA) = true; i--; continue; }
        e.h(useB ? 0xB3B4 : 0xB3C6, 0, 0, 0, 0, 0, u, v, k);
        if (useB) { if (++kB >= 3) doneB = true; } else if (++kA >= 3) doneA = true;
    }
    if (!kA && !kB) return 0;
    e.h(0xA65E, 0, 0, 0, 0, 0, u, 0x10, 0);
    for (uint32_t i = 0; i < 3; i++) {
        uint32_t s = e.h(0xB3AE, 0, 0, 0, 0, 0, e.h(0xA9C4, 0, 0, 0, 0, 0, u, i) & 0xFFFF);
        if (s) e.h(0xA9D0, 0, 0, 0, 0, 0, 0, 0, u, s, 0, 0);
        s = e.h(0xB3AE, 0, 0, 0, 0, 0, e.h(0xA90A, 0, 0, 0, 0, 0, u, i) & 0xFFFF);
        if (s) e.h(0xA9D0, 0, 0, 0, 0, 0, 0, 0, u, s, 0, 0);
    }
    e.h(0x13DE0, u, 0, 0, 0, 0, p);
    return 1;
}
}
