// 1.13c D2Game+0x5A9B0 (ACT5Q4 monster-death callback): ecx = quest node, edx = callback args, plain ret, no result.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape). a = args+0x14, b = args+0x18, unit = args+0xC.
// b == 0x6D: 0x58A40 (eax = unit, stack args word 0; ret 4). a != 0x6D skips to the tail. Node word +0x9C nonzero: 0x33830 (eax = unit's
// +0xC word, -1 without a unit; ecx = node+0x1C; plain ret). Node byte +0xC == 2: rec = import 0xA4C0(unit); t = rec word
// [+0x10 + 4 * (args word 0 byte +0x6D)]; import 0xA568(t, node+0xE0 word, 0) == 1 or (..., 1) == 1 returns. Else 0x35EB0 (eax = 3,
// esi = node); node byte +0xB != 1: +0x14 := 0, +0xB := 1, 0xDFCC0 (eax 0, ebx = module 0x58D40, edi = node+4); then always 0xDFCC0
// with ebx = module 0x57320. Tail: b >= 0x83 (signed), node byte +9 set and +0xB below 2: +0x14 := 0, +0xB := 2 and the 0x58D40 call; any
// b >= 0x83 ends with the 0x57320 call. 0xDFCC0 takes stack 0 (ret 4).
#pragma once
#include <cstdint>

namespace d2g5a9b0 {
template <class E> void dfccPair(E& e, uint32_t node, bool first)
{
    if (first) { const uint32_t g = e.rd32(node + 4); e.h(0xDFCC0, 0, 0, 0, 0xB2058D40u, 0, 0); e.edi(0xDFCC0, g); }
    const uint32_t g = e.rd32(node + 4);
    e.h(0xDFCC0, 0, 0, 0, 0xB2057320u, 0, 0);
    e.edi(0xDFCC0, g);
}
template <class E> void MonDeath(E& e, uint32_t node, uint32_t msg)
{
    const int32_t b = (int32_t)e.rd32(msg + 0x18);
    const uint32_t a = e.rd32(msg + 0x14), unit = e.rd32(msg + 0xC);
    if (b == 0x6D) e.h(0x58A40, unit, 0, 0, 0, 0, e.rd32(msg));
    if (a == 0x6D) {
        if ((e.rd16s(node + 0x9C) & 0xFFFF) != 0) e.h(0x33830, unit ? e.rd32(unit + 0xC) : 0xFFFFFFFFu, node + 0x1C);
        if (e.rd8(node + 0xC) == 2) {
            const uint32_t game = e.rd32(msg);
            const uint32_t rec = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
            const uint32_t t = e.rd32(rec + 0x10 + 4 * e.rd8(game + 0x6D));
            if (e.h(0xA568, 0, 0, 0, 0, 0, t, e.rd32(node + 0xE0), 0) == 1) return;
            if (e.h(0xA568, 0, 0, 0, 0, 0, t, e.rd32(node + 0xE0), 1) == 1) return;
            e.h(0x35EB0, 3, 0, 0, 0, node);
            const bool first = e.rd8(node + 0xB) != 1;
            if (first) { e.wr8(node + 0x14, 0); e.wr8(node + 0xB, 1); }
            dfccPair(e, node, first);
        }
    }
    if (b < 0x83) return;
    const bool first = e.rd8(node + 9) && e.rd8(node + 0xB) < 2;
    if (first) { e.wr8(node + 0x14, 0); e.wr8(node + 0xB, 2); }
    dfccPair(e, node, first);
}
}
