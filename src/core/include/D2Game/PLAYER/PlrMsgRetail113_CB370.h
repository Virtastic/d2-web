// 1.13c D2Game+0xCB370: EAX = a, stack word = b (ret 4). Per unit, a first: #10459(#10826(#10331(u))) through
// thunks 0xA478, 0xA556, 0xA56E (stdcall 1 each). Returns 0 when the two low bytes match, else 2 (the whole EAX).
#pragma once
#include <cstdint>
namespace gcb370 {
template <class E> uint32_t Rule(E& e, uint32_t a, uint32_t b)
{
    // 1.13c D2Game+0xCB370
    auto key = [&](uint32_t u) {
        const uint32_t x = e.h(0xA478, 0, 0, 0, 0, 0, u);
        const uint32_t y = e.h(0xA556, 0, 0, 0, 0, 0, x);
        return e.h(0xA56E, 0, 0, 0, 0, 0, y) & 0xFF;
    };
    const uint32_t x = key(a);
    return (x == key(b)) ? 0 : 2;
}
}
