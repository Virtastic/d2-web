// D2DDraw statically linked CRT helper: shift the 96-bit mantissa right.
#include <cstdint>

// 1.13c D2DDraw+0x09C47 __ShrMan: shift the three dwords man[0..2] (man[0] most significant) right by n bits.
// First a sub-dword shift by n % 32 carried across the words, then a whole-dword move by n / 32 with zero fill.
extern "C" void d2ddraw_shr_man(uint32_t* man, int32_t n) {
    uint32_t s = (uint32_t)(n % 32) & 0x1f;
    uint32_t carry = 0;
    for (int i = 0; i < 3; ++i) {
        uint32_t v = man[i];
        man[i] = (v >> s) | carry;
        carry = (v & ~(0xFFFFFFFFu << s)) << ((0x20 - s) & 0x1f);
    }
    int32_t w = n / 32;
    for (int i = 2; i >= 0; --i) man[i] = (i < w) ? 0 : man[i - w];
}
