// 1.13c D2Game+0x463F0 (skill start that opens a timed state record), transcribed from the 1.13c disassembly. Convention: ESI = unit,
// stack = (game, skill id, level), ret 0xC. Returns 0 when the id is out of range, 0x9EE50 fails (< 0), the record byte +0xBE is not a
// valid index below dt+0xBF0, or 0xA3610 returns 0; otherwise 1. Callees as retail passes them: 0x9EE50 (EAX = ECX = 0, EDX = &out,
// stack unit, 0, id, level, ret 0x10), 0xAC9A (unit, calc, id, level), 0xA3610 (EAX = &9-word block, stack game; the block is logged
// after the call), 0xEFE40 (ECX = game, EBX = unit+0xD0, stack 0), 0x4E2A0 (EAX = unit, ECX = max(1, AC9A(rec+0x13C)), stack game,
// 0xA3610 result, level) and 0x4F970 (game, unit, 0xA3610 result, id, level, 0).
#pragma once
#include <cstdint>

namespace r463f0 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t unit, uint32_t game, int32_t id, uint32_t level)
{
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)id * 0x23C + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    volatile uint32_t l0 = 0;
    const int32_t r9 = (int32_t)e.h(0x9EE50, 0, 0, e.stk((const void*)&l0), 0, 0, unit, 0, (uint32_t)id, level);
    if (r9 < 0) return 0;
    const int32_t cl = (int8_t)e.rd8(rec + 0xBE);
    if (cl < 0 || cl >= (int32_t)e.rd32(dt + 0xBF0)) return 0;
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    const uint32_t ac = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0xC0), (uint32_t)id, level);
    uint32_t b[9] = {};
    b[1] = unit;
    b[2] = (uint32_t)r9;
    b[4] = l0;
    b[7] = (uint32_t)cl;
    b[8] = ac;
    const uint32_t blkp = e.h(0xA3610, e.stk(b), 0, 0, 0, 0, game);
    e.blk(0xA3610, b, 9);
    if (!blkp) return 0;
    e.h(0xEFE40, 0, game, 0, e.rd32(unit + 0xD0), 0, 0);
    int32_t v = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x13C), (uint32_t)id, level);
    if (v < 1) v = 1;
    e.h(0x4E2A0, unit, (uint32_t)v, 0, 0, 0, game, blkp, level);
    e.h(0x4F970, level, 0, 0, 0, 0, game, unit, blkp, (uint32_t)id);
    return 1;
}
}
