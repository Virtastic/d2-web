// 1.13c D2CMP+0x9C90 (28 bytes): fill the squares table, d*d for d = -256..255.
#include "d2cmp_palstate.h"
int32_t g_d2cmp_dist_6fe875f0;  // 0x6FE875F0, entry d = -256 (below the 511-entry table 0x9A10 reads)
// 1.13c D2CMP+0x09C90
extern "C" void d2cmp_9c90(void) {
    g_d2cmp_dist_6fe875f0 = 0x10000;
    for (int32_t d = -255; d < 0x100; ++d) g_d2cmp_dist_6fe879f0[255 + d] = d * d;
}
