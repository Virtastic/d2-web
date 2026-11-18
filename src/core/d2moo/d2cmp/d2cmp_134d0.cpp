// D2CMP Codec.cpp: read n bits from a DCC bit stream (LSB first).
#include "d2cmp_dcc.h"

// 1.13c D2CMP+0x134D0
// Retail: EAX = n, EDX = stream; n == 0 returns 0 without touching the stream.
uint32_t d2cmp_134d0(int32_t n, DccBits* b) {
    if (n == 0) return 0;
    return dcc_read(b, n);
}
