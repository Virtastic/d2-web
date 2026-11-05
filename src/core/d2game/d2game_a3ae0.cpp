// 1.13c D2Game+0xA3AE0 -- EAX = unit, ECX = p2, stack pp (ret 4). stat = 1, or D2Game+0xA400(unit, p2, 1) (stdcall, 3 words)
// when unit is set; result = (*pp)->+0x1C / 3 + stat / 2 (both truncating toward zero), at least 1.
#pragma once
#include <cstdint>
namespace ga3ae0 {
// 1.13c D2Game+0xA3AE0
template <class E>
int32_t Level(E& e, uint32_t unit, uint32_t p2, uint32_t pp)
{
    const int32_t stat = unit ? (int32_t)e.h(0xA400, 0, 0, 0, 0, 0, unit, p2, 1) : 1;
    const int32_t v = (int32_t)E::rd32(E::rd32(pp) + 0x1C) / 3 + stat / 2;
    return v > 1 ? v : 1;
}
}
