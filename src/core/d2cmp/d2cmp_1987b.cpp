#include <cstdint>
// 1.13c D2CMP+0x1987B __RoundMan: round-half-even at bit `bit`; clears the bits below it and returns the carry from __IncMan.
extern "C" uint32_t d2cmp_197fc(const uint32_t* man, int bit);
extern "C" int d2cmp_1982e(uint32_t* man, int bit);
extern "C" int d2cmp_1987b(uint32_t* man, int bit)
{
    int ret = 0;
    int w = bit / 32;
    unsigned sh = (0x1f - (bit % 32)) & 0x1f;
    if ((man[w] & (1u << sh)) && d2cmp_197fc(man, bit) == 0)
        ret = d2cmp_1982e(man, bit - 1);
    man[w] &= 0xFFFFFFFFu << sh;
    for (int i = w + 1; i < 3; ++i) man[i] = 0;
    return ret;
}
