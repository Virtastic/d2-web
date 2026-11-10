// 1.13c D2Game+0x76570 (unit timer expiry gate): ECX = game, EDX = unit, two unused stack words (ret 8). Rule over a hook environment
// (see src/core/fn/hookenv.h). Written from the 1.13c disassembly.
// A null unit or a null timer handle at unit +0xA4 returns. due = 0xAB3E(handle) (D2Common 11067). due <= game +0xA8: 0xAAF6(game +0x1C, handle) (10422),
// unit +0xA4 := 0, 0xA4DE(unit) (10158), unit +0xC4 |= 0x100. Otherwise 0x8EA80 (ECX = game, EDX = unit, stack 6, due, 0, 0, 0) requeues.
#pragma once
#include <cstdint>

namespace g76570 {
template <class E> void TimerExpire(E& e, uint32_t game, uint32_t unit)
{
    if (!unit) return;
    const uint32_t ev = e.rd32(unit + 0xA4);
    if (!ev) return;
    const uint32_t due = e.h(0xAB3E, 0, 0, 0, 0, 0, ev);
    if (due <= e.rd32(game + 0xA8)) {
        e.h(0xAAF6, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), ev);
        e.wr32(unit + 0xA4, 0);
        e.h(0xA4DE, 0, 0, 0, 0, 0, unit);
        e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x100);
        return;
    }
    e.h(0x8EA80, 0, game, unit, 0, 0, 6, due, 0, 0, 0);
}
}
