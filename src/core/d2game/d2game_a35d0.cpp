// 1.13c D2Game+0xA35D0 -- EAX = game (null is retail's fatal assert), one stack word p1 (ret 4).
// Forwards (game+0x80, p1) to D2Game+0xE2600 (stdcall, 2 words) and returns its EAX.
// E: h(rva, eax, ecx, edx, ebx, esi, s0..) calls a callee; rd32(addr) reads a retail dword.
#pragma once
#include <cstdint>
#include <cstdlib>
namespace ga35d0 {
// 1.13c D2Game+0xA35D0
template <class E>
uint32_t Forward(E& e, uint32_t game, uint32_t p1)
{
    if (!game) std::abort();
    return e.h(0xE2600, 0, 0, 0, 0, 0, E::rd32(game + 0x80), p1);
}
}
