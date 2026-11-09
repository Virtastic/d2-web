// 1.13c D2Game+0x3C5B0 (a missile collide/hit callback): ecx = pGame, edx = missile unit, one unused stack word, ret 4, result 1.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape; E::blk logs the request block, E::modAddr(rva) a
// module address as the log records it). Rule: a null unit, class (+4) outside [0, missiles count), a null record or rec.s16[+0x24]
// negative -> 1. Unit +0xC8 bit 10 clear -> 1. t = 0xE03A0 (eax = unit +0x94, ecx = game,
// edx = unit +0x98; plain ret); null -> 1. A zeroed 23-dword
// request: [0] 3, [1] t, [4] rec.s16[+0x24], [5] 0x1100(ecx = unit), [6] 0x1130(ecx = unit), [0xB] import 0xB26A(unit), [0xC] import
// 0xB0FC(unit), [0x15] module 0x39210. v = (0xB0FC(unit) - 1) * rec[+0x50] + rec[+0x4C]; v below 1: s = 0xB26A(unit), l = 0xB0FC(unit),
// k = 0x1420(eax = s; plain ret), null -> 1; v = import 0xAC9A(t, k +0x144, s, l), at least 1. Then 0xA26E0 (esi = &request, stack
// game, v; ret 8).
#pragma once
#include <cstdint>

namespace d2g3c5b0 {
template <class E> uint32_t MissHit(E& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    if (!unit) return 1;
    const int32_t cls = (int32_t)e.rd32(unit + 4);
    if (cls < 0 || cls >= (int32_t)e.rd32(dt + 0xB6C)) return 1;
    const uint32_t rec = (uint32_t)cls * 0x1A4u + e.rd32(dt + 0xB64);
    if (!rec || e.rd16s(rec + 0x24) < 0) return 1;
    if (!((e.rd32(unit + 0xC8) >> 10) & 1)) return 1;
    const uint32_t t = e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
    if (!t) return 1;
    uint32_t req[23] = {};
    req[0] = 3;
    req[5] = e.h(0x1100, 0, unit);
    req[6] = e.h(0x1130, 0, unit);
    req[1] = t;
    req[4] = (uint32_t)e.rd16s(rec + 0x24);
    req[0xB] = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    req[0xC] = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    req[0x15] = e.modAddr(0x39210);
    int32_t v = ((int32_t)e.h(0xB0FC, 0, 0, 0, 0, 0, unit) - 1) * (int32_t)e.rd32(rec + 0x50) + (int32_t)e.rd32(rec + 0x4C);
    if (v < 1) {
        const uint32_t s = e.h(0xB26A, 0, 0, 0, 0, 0, unit);
        const uint32_t l = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
        const uint32_t k = e.h(0x1420, s);
        if (!k) return 1;
        v = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, t, e.rd32(k + 0x144), s, l);
        if (v < 1) v = 1;
    }
    e.h(0xA26E0, 0, 0, 0, 0, e.stk(req), game, (uint32_t)v);
    e.blk(0xA26E0, req, 23);
    return 1;
}
}
