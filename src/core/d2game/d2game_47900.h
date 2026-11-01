// 1.13c D2Game+0x47900 (query a position, then place a unit there), transcribed from the 1.13c disassembly. Convention: EAX = first value,
// ESI = unit, EDI = value, stack = (word), ret 4. 0x471E0 (stack word, EAX-in, 0, EDI, ESI, &local, &argslot; ret 0x1C) fills a local and
// the caller's own argument slot as two outputs; a zero result returns 0. Otherwise 0xEF940 (EAX = ESI, EBX = 1, ECX = word, EDX = the
// argument-slot output, stack EAX-in, local output, slot output, 0, EDI, 0; ret 0x18) and its result is returned.
#pragma once
#include <cstdint>

namespace r47900 {
template <class E> uint32_t Do(E& e, uint32_t eaxIn, uint32_t unit, uint32_t val, uint32_t w)
{
    volatile uint32_t loc = 0, slot = w;
    if (!e.h(0x471E0, 0, 0, 0, 0, 0, w, eaxIn, 0, val, unit, e.stk((const void*)&loc), e.stk((const void*)&slot))) return 0;
    return e.h(0xEF940, unit, w, slot, 1, 0, eaxIn, loc, slot, 0, val, 0);
}
}
