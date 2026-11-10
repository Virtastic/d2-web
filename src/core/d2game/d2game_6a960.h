// 1.13c D2Game+0x6A960: ECX, EDX, five stack words (ret 0x14). Written from the 1.13c disassembly
// as a rule over E (h = hooked callee, see src/core/fn/hookenv.h). Rule: a zeroed 9-dword local block gets
// {ECX, EDX, 0, 0, s0, s1, s2, s3, s4}; 0xA00B0 is called with EAX = &block (plain ret) and its EAX is the result.
#pragma once
#include <cstdint>

namespace g6a960 {
template <class E> uint32_t PackAndCall(E& e, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4)
{
    // 1.13c D2Game+0x6A960
    volatile uint32_t blk[9] = { ecx, edx, 0, 0, s0, s1, s2, s3, s4 };
    const uint32_t p = (uint32_t)(uintptr_t)blk;
    const uint32_t r = e.h(0xA00B0, p);
    e.blk(0xA00B0, p, 9);
    return r;
}
}
