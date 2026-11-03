#include <cstdint>
// 1.13c D2CMP+0x1982E __IncMan: add 1 at mantissa bit `bit` with carry toward the top word; returns the final carry.
static int addl_c(uint32_t a, uint32_t b, uint32_t* out) { *out = a + b; return *out < a; }
extern "C" int d2cmp_1982e(uint32_t* man, int bit)
{
    uint32_t* p = man + bit / 32;
    int carry = addl_c(*p, 1u << ((0x1f - (bit % 32)) & 0x1f), p);
    for (int i = bit / 32 - 1; i >= 0; --i) {
        if (!carry) return carry;
        carry = addl_c(man[i], 1, &man[i]);
    }
    return carry;
}
