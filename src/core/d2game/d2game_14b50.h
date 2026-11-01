// 1.13c D2Game+0x14B50: affix-roll dispatcher. esi = item, ebx and edi pass through, stack = (a1, a2, a3, a4), ret 0x10.
// The item format (D2Common #10718 via thunk 0xB37E, a 16-bit result) picks the roller: format >= 1 runs 0x144D0
// (stdcall: item, a1, a2, a3, ebx, edi, a4), format 0 runs 0x148A0 (stdcall: item, a1, a2, a3, ebx, edi); its result is returned.
#pragma once
#include <cstdint>

namespace d2g14b50 {
template <class E> uint32_t Dispatch(E& e, uint32_t item, uint32_t ebx, uint32_t edi, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
    const uint16_t fmt = (uint16_t)e.h(0xB37E, 0, 0, 0, 0, 0, item);
    if (fmt >= 1) return e.h(0x144D0, 0, 0, 0, ebx, item, item, a1, a2, a3, edi);
    (void)a4;
    return e.h(0x148A0, 0, 0, 0, ebx, item, item, a1, a2, a3, edi);
}
}
