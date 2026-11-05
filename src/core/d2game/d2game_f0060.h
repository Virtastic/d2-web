// 1.13c D2Game+0xF0060: ESI = unit, EDI = other. pd = #10920(EDI) through thunk 0xA4C0 (stdcall 1, always called);
// id = unit ? unit+0xC : -1. When EDI != ESI the list at pd+0x38 (next at +0x10) is walked for the node whose dword 0 is id; it is returned, else 0.
// D2Game+0xF0220 is the same lookup, then a found node (EDI != ESI) takes the stack word at +0xC (ret 4).
#pragma once
#include <cstdint>
namespace gf0060 {
template <class E> uint32_t Find(E& e, uint32_t esi, uint32_t edi)
{
    // 1.13c D2Game+0xF0060
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, edi);
    const uint32_t id = esi ? e.rd32(esi + 0xC) : 0xFFFFFFFFu;
    if (edi != esi)
        for (uint32_t n = e.rd32(pd + 0x38); n; n = e.rd32(n + 0x10))
            if (e.rd32(n) == id) return n;
    return 0;
}
template <class E> uint32_t SetState(E& e, uint32_t esi, uint32_t edi, uint32_t v)   // EAX is left holding the node (or 0)
{
    // 1.13c D2Game+0xF0220
    const uint32_t n = Find(e, esi, edi);
    if (edi != esi && n) e.wr32(n + 0xC, v);
    return n;
}
}
