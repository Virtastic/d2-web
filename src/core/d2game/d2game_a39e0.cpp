// 1.13c D2Game+0xA39E0 -- ECX unused, EDX = p2, stack rec and lim (ret 8).
// rec must be a type-2 unit of class 0x233; then d = D2Game+0xAB86(rec, p2) (stdcall, 2 words) and the result is rec
// when d <= lim[1] (signed), else 0.
#pragma once
#include <cstdint>
namespace ga39e0 {
// 1.13c D2Game+0xA39E0
template <class E>
uint32_t Gate(E& e, uint32_t p2, uint32_t rec, uint32_t lim)
{
    if (!rec || E::rd32(rec) != 2 || E::rd32(rec + 4) != 0x233) return 0;
    const int32_t d = (int32_t)e.h(0xAB86, 0, 0, 0, 0, 0, rec, p2);
    return d > (int32_t)E::rd32(lim + 4) ? 0 : rec;
}
}
