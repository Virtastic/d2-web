// 1.13c D2Game+0xC2120: EAX = p, returns AL. The list head is (p+4)->+0x10F4; null returns 0. Entries
// start at *head and chain through +0xF4; the first entry whose dword 0 is 10 answers the byte at (entry+0x18)->+4; none returns 0.
#pragma once
#include <cstdint>
namespace gc2120 {
template <class E> uint32_t Rule(E& e, uint32_t p)
{
    // 1.13c D2Game+0xC2120
    const uint32_t hd = e.rd32(e.rd32(p + 4) + 0x10F4);
    if (!hd) return 0;
    for (uint32_t n = e.rd32(hd); n; n = e.rd32(n + 0xF4))
        if (e.rd32(n) == 10) return e.rd8(e.rd32(n + 0x18) + 4);
    return 0;
}
}
