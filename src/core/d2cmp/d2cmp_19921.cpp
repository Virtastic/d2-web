#include <cstdint>
// 1.13c D2CMP+0x19921 __ShrMan: shift the 3-dword mantissa right by n bits.
extern "C" void d2cmp_19921(uint32_t* man, int n)
{
    unsigned s = n % 32;
    uint32_t carry = 0;
    for (int i = 0; i < 3; ++i) {
        uint32_t v = man[i];
        man[i] = (v >> s) | carry;
        carry = (v & ~(0xFFFFFFFFu << s)) << ((0x20 - s) & 0x1f);
    }
    const uint32_t* src = man + (2 - n / 32);
    for (int i = 2; i >= 0; --i, --src)
        man[i] = (i < n / 32) ? 0 : *src;
}
