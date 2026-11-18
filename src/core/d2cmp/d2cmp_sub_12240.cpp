#include <cstdint>
extern "C" __attribute__((weak)) const uint32_t d2cmp_tbl_6fe2e4d8[];   // 0x6FE2E4D8, rows of 64 dwords

// 1.13c D2CMP+0x12240
// Row 0 when mask==0, else row (lowest set bit of mask)+1; returns table[row*0x40 + k].
uint32_t d2cmp_12240(int k, uint32_t mask)
{
    int row = 0;
    if (mask) { while (!((mask >> row) & 1)) row++; row++; }
    return d2cmp_tbl_6fe2e4d8[row * 0x40 + k];
}
