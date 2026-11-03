#include <cstdint>
// 1.13c D2CMP+0x197FC __ZeroTail: 1 when every mantissa bit below `bit` is zero (man = 3 dwords, top word first).
extern "C" uint32_t d2cmp_197fc(const uint32_t* man, int bit)
{
    int i = bit / 32;
    uint32_t v = man[i] & ~(0xFFFFFFFFu << ((0x1f - (bit % 32)) & 0x1f));
    for (;;) {
        if (v != 0) return 0;
        if (++i > 2) return 1;
        v = man[i];
    }
}
