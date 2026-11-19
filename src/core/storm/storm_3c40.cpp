// 1.13c Storm+0x3C40 (EAX = wanted length, stack = the vector): the default grain of a bignum vector. From 0x40 up the grain 0x40 is
// stored at +0xC and returned; below, the highest power of two <= n (1 for 0).
#include <cstdint>
extern "C" uint32_t Storm13c_3C40(uint32_t n, uint32_t* vec) {
    if (n >= 64) { vec[3] = 64; return 64; }
    if (n == 0) return 1;
    while (n & (n - 1)) n &= n - 1;
    return n;
}
