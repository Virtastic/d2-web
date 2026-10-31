// 1.13c D2CMP+0x9C60 (export #10029, 33 bytes): set 0x9A10's "previous colour", byte-swapped to 0xRRGGBB.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x09C60
extern "C" void d2cmp_9c60(uint32_t c) {
    g_d2cmp_lastColor_6fe8831c = ((c & 0xffu) << 16) | (c & 0xff00u) | ((c >> 16) & 0xffu);
}
