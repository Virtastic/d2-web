// 1.13c D2Game+0xB5950 (PickRandomOfFiveAndQueue): ecx = pGame, edx = pUnit, stack = (unused, arg4), ret 8.
// Rule over an environment E (see d2game_c310.h): every callee that is not a field read is one E::h call.
// Roll the unit's LCG (unit+0x20 lo, +0x24 hi := lo*0x6ac690c5 + hi, 64-bit), pick skill id = table[low32 % 5]
// (DAT_6fd1b454, the five ids 0x42 0x48 0x52 0x57 0x5B); id, row and class-index checks fail with 0; set unit+0xC4 |= 0x40;
// eval the row's calc +0x64 (word 2 of the call) and +0x60 (block word 6) through Ordinal 10786 (0xAC9A, 4 words); build a
// 26-dword skill request block (all zero, -1 at words 7..12, per-pick fields) and hand (3, unit, calc1, 0x50250, &block)
// to 0xA0AD0 (ecx = game, ret 0x14); its result is ours.
#pragma once
#include <cstdint>

namespace g5950 {
template <class E> uint32_t Pick(E& e, uint32_t dt, uint32_t tbl, uint32_t game, uint32_t unit, uint32_t p4)
{
    const uint64_t v = (uint64_t)e.rd32(unit + 0x20) * 0x6ac690c5ull + e.rd32(unit + 0x24);
    E::wr32(unit + 0x20, (uint32_t)v);
    E::wr32(unit + 0x24, (uint32_t)(v >> 32));
    const uint32_t idx = (uint32_t)v % 5u;
    const int32_t id = (int32_t)e.rd32(tbl + 4 * idx);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)id * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const int32_t cls = e.rd16s(rec + 0x82);
    if (cls < 0 || cls >= (int32_t)e.rd32(dt + 0xC4)) return 0;
    if (unit) E::wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    const uint32_t r1 = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x64), (uint32_t)id, p4);
    const uint32_t r2 = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x60), (uint32_t)id, p4);
    uint32_t S[26] = {};
    S[0] = game; S[1] = unit; S[4] = (uint32_t)id; S[5] = p4; S[6] = r2;
    for (int i = 7; i <= 12; i++) S[i] = 0xFFFFFFFFu;
    S[19] = (uint32_t)cls;
    switch (idx) {
    case 0: S[7] = 0x24; S[13] = (uint32_t)-100; break;
    case 1: S[7] = 0x19; S[13] = (uint32_t)-50; break;
    case 2: S[20] = 5; S[23] = 4; break;
    case 3: S[20] = 1; S[23] = 5; S[21] = 2; S[24] = 5; break;
    case 4: {
        const uint32_t g = e.rd32(rec + 0x160);
        S[3] = 1; S[7] = 0x43; S[13] = g; S[14] = (uint32_t)-50; S[8] = 0x19;
        S[15] = (uint32_t)-50; S[9] = 0x24; S[10] = 0x44; S[16] = g;
        break; }
    }
    const uint32_t ret = e.h(0xA0AD0, 0, game, 0, 0, 0, 3, unit, r1, 0xB2050250u);
    e.blk(0xA0AD0, S, 26);
    return ret;
}
}
