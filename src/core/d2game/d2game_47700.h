// 1.13c D2Game+0x47700 (create a randomly repeated unit near a spot), transcribed from the 1.13c disassembly. Convention: EAX = first
// value, EBX = a value kept in the calls, stack = (game, word, low byte, high byte), ret 0x10. Both bytes must be non-zero with
// high >= low, else 0. 0x47450 (stack game, EAX-in, word, EBX, &out1, &out0 = the caller's own first argument slot, 1; ret 0x1C; zero
// returns 0) fills the two outputs; 0xEF510 (EAX = EBX, ECX = 1, EDX = EAX-in, stack game, word, out1, out0, -1, 0; zero returns 0)
// makes the unit. If 0x13B0(EAX = EBX, EDX = 0x12) is non-zero and 0x1850(EAX = EBX) is 0x210, the unit's 0x1130 and 0x1100 values go to
// 0xE1D90 (ECX = 2, EDX = 0x232, stack those two, game, EAX-in, 1, 0, 0). Then 0x11D0 (ECX = unit + 0x20, ESI = high - low + 1) gives
// a random number; (low - 1 + it) times, while positive, 0xEF570 (EAX = EBX, ECX = 1, EDX = unit, stack game, word, 3, 0). Returns the unit.
#pragma once
#include <cstdint>

namespace r47700 {
template <class E> uint32_t Do(E& e, uint32_t eaxIn, uint32_t ebx, uint32_t game, uint32_t w, uint32_t lowW, uint32_t highW)
{
    const uint32_t lo = lowW & 0xFF, hi = highW & 0xFF;
    if (!lo || !hi || hi < lo) return 0;
    volatile uint32_t out1 = 0, out0 = game;
    if (!e.h(0x47450, 0, 0, 0, 0, 0, game, eaxIn, w, ebx, e.stk((const void*)&out1), e.stk((const void*)&out0))) return 0;
    const uint32_t u = e.h(0xEF510, ebx, 1, eaxIn, 0, 0, game, w, out1, out0, 0xFFFFFFFFu);
    if (!u) return 0;
    const uint32_t mn = (lo - 1) & 0xFF, mx = (hi - 1) & 0xFF;
    if (e.h(0x13B0, ebx, 0, 0x12)) {
        if (e.h(0x1850, ebx) == 0x210) {
            const uint32_t r30 = e.h(0x1130, 0, u);
            const uint32_t r00 = e.h(0x1100, 0, u);
            e.h(0xE1D90, 0, 2, 0x232, 0, 0, r00, r30, game, eaxIn);
        }
    }
    int32_t n = (int32_t)e.h(0x11D0, 0, u + 0x20, 0, 0, mx - mn + 1) + (int32_t)mn;
    for (; n > 0; --n) e.h(0xEF570, ebx, 1, u, 0, 0, game, w, 3, 0);
    return u;
}
}
