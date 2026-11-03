// 1.13c D2Game+0x5B030 ACT5Q3_PrisonOfIceKeyCheck): EAX = event unit, stack word = quest record (ret 4).
// Returns true when the global 'ice ' key check (0xE660) passes, else true only if the unit is armed (+0x80 set, word 0 clear), 0x9BEB0 says its
// short result is not -1, and the registered callback (module +0x57D80, run by 0x9C160 with ESI = the game at quest+4, EBX = 0) sets state+0xE2.
// Every callee is a seam on E: E::h(rva, eax, ecx, edx, ebx, esi, stack words...) and E::rd32/rd8/wr8.
#pragma once
#include <cstdint>

namespace d2g5b030 {
template <class E> bool Run(E& e, uint32_t unit, uint32_t quest, uint32_t mod)
{
    if (e.h(0xE660, 0, 0, 0, 0, 0, 0x20656369) != 0) return true;
    const uint32_t st = e.rd32(quest + 0x18);
    if (unit == 0 || e.rd32(unit + 0x80) == 0 || e.rd32(unit) != 0) return false;
    if ((int16_t)e.h(0x9BEB0, unit) == -1) return false;
    e.wr8(st + 0xE2, 0);
    e.h(0x9C160, 0, 0, 0, 0, e.rd32(quest + 4), mod + 0x57D80);
    return e.rd8(st + 0xE2) != 0;
}
}
