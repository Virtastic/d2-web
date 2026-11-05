// 1.13c D2Game+0xC2460: EAX = game, returns AL. head = game+0x10F4; entries start at *head and chain
// through +0xF4. The first entry whose dword 0 is 8 decides: byte +9 clear returns 1, else (byte +0xC == 5). No head or no such entry returns 1.
#pragma once
#include <cstdint>
namespace gc2460 {
template <class E> uint32_t Rule(E& e, uint32_t game)
{
    // 1.13c D2Game+0xC2460
    const uint32_t hd = e.rd32(game + 0x10F4);
    if (hd)
        for (uint32_t n = e.rd32(hd); n; n = e.rd32(n + 0xF4))
            if (e.rd32(n) == 8) {
                if (e.rd8(n + 9)) return e.rd8(n + 0xC) == 5;
                break;
            }
    return 1;
}
}
