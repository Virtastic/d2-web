// 1.13c D2Common+0x4DD0 __aulldiv: unsigned EDX:EAX quotient of two retail word pairs. D2MOO has no twin (compiler helper).
#include <cstdint>

// 1.13c D2Common+0x4DD0
extern "C" uint64_t d2common_aulldiv_4dd0(uint32_t low, uint32_t high, uint32_t divisorLow, uint32_t divisorHigh)
{
    const uint64_t numerator = (uint64_t{high} << 32) | low;
    const uint64_t denominator = (uint64_t{divisorHigh} << 32) | divisorLow;
    // The x86 DIV faults for zero; avoid C++ undefined behavior on that path.
    if (denominator == 0) __builtin_trap();
    return numerator / denominator;
}
