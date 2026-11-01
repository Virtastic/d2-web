// 1.13c D2Game+0x44C20 (druid skill start, Rabies shape), transcribed from the 1.13c disassembly. Convention: ECX = game,
// EDX = unit, stack = (skill id, skill level), ret 8. Every callee is one E::h(rva, eax, ecx, edx, ebx, esi, stack words...)
// call as retail passes it. The source-file string pointer handed to 0xA49C is logged as 0xB2000000|rva (the module convention).
#pragma once
#include <cstdint>

namespace r44c20 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t id, uint32_t level)
{
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)id * 0x23C + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const uint32_t sk = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!sk) return 0;
    if ((int32_t)e.h(0xA49C, 0, 0, 0, 0, 0, sk, 0xB2000000u | 0xFC3A8, 0x2F1) != id) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit || !tgt) return 0;
    const int16_t st = (int16_t)e.rd16s(rec + 0x80);
    if (st >= 0 && st < (int32_t)e.rd32(dt + 0xC4) && !e.h(0xA68E, 0, 0, 0, 0, 0, unit, (uint32_t)(int32_t)st)) return 0;
    e.h(0xAC94, 0, 0, 0, 0, 0, sk, 0);
    const uint32_t hit = e.h(0xACD6, 0, 0, 0, 0, 0, unit, (uint32_t)id, level);
    const uint32_t fl = e.h(0xDE5A0, tgt, 0, 0, 0, 0, game, hit, 0) & 0xFFFF;
    if (!(fl & 1)) return 0;
    e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x138), (uint32_t)id, level);
    if (e.rd8(rec + 0x1DC)) e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x144), (uint32_t)id, level);
    e.h(0xAC94, 0, 0, 0, 0, 0, sk, 1);
    return 1;
}
}
