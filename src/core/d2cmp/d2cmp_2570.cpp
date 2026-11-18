// 1.13c D2CMP+0x2570 (EAX = n, stdcall arr): growth granule for the bucket array. n < 0x15: the highest
// set bit of n (1 when n is 0). Otherwise 0x15, also stored as the array's fixed granule (+0xc).
#include "d2cmp_guest.h"
using namespace d2cmpg;
// 1.13c D2CMP+0x2570
extern "C" uint32_t d2cmp_2570(uint8_t* m, uint32_t n, uint32_t arr) {
    if (n >= 0x15) { wr(m, arr + 0xc, 0x15); return 0x15; }
    while (n & (n - 1)) n &= n - 1;
    return n < 1 ? 1 : n;
}
