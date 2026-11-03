// 1.13c D2Game+0x65500 (quest-state byte for a quest record): ecx = Q (the quest record), edx = unit, stack = (unused, questFlags, byte *out), ret 0xC, returns 1.
// Written from the 1.13c disassembly as a rule over an environment E (every callee is one E::h(rva, ...) call). *out = 0 first; the tests are
// 0xA568(flags, 3, n) (a quest-flag bit test). n = 1 set: 0xA. Else 0x64B60(eax = unit; Q) non-zero: out = 2 when bit 0 is clear. Else when byte Q+9
// is set: bit 0xD set: out = 0xA when bit 1 is set, else 0xD (asm: neg/sbb/and 0xFD/add 0xD); else bit 0xE set: 0xC; else when byte Q+0xC < 5: out = byte Q+0xB;
// else when 0x338D0(eax = dword Q+4; 3, 0xD) is non-zero: out = 4 + 8 * (0xA3E2(unit, 0xC, 0) >= 8, signed).
#pragma once
#include <cstdint>

namespace r65500 {
template <class E> uint32_t QuestState(E& e, uint32_t Q, uint32_t unit, uint32_t flags, uint32_t out)
{
    e.wr8(out, 0);
    auto bit = [&](uint32_t n) { return e.h(0xA568, 0, 0, 0, 0, 0, flags, 3, n); };
    if (bit(1)) { e.wr8(out, 0xA); return 1; }
    if (e.h(0x64B60, unit, 0, 0, 0, 0, Q)) {
        if (!bit(0)) e.wr8(out, 2);
        return 1;
    }
    if (!e.rd8(Q + 9)) return 1;
    if (bit(0xD)) { e.wr8(out, bit(1) ? 0xA : 0xD); return 1; }
    if (bit(0xE)) { e.wr8(out, 0xC); return 1; }
    if (e.rd8(Q + 0xC) < 5) { e.wr8(out, e.rd8(Q + 0xB)); return 1; }
    if (e.h(0x338D0, e.rd32(Q + 4), 0, 0, 0, 0, 3, 0xD))
        e.wr8(out, (int32_t)e.h(0xA3E2, 0, 0, 0, 0, 0, unit, 0xC, 0) >= 8 ? 0xC : 4);
    return 1;
}
}
