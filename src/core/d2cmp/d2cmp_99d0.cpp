// 1.13c D2CMP+0x99D0 (export #10033, 33 bytes): install the 256-byte exclusion table for 0x9A10.
#include <cstring>
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x099D0
extern "C" void d2cmp_99d0(const uint8_t* table) {
    std::memcpy(g_d2cmp_excl_6fe87df0, table, 0x100);
    g_d2cmp_useExcl_6fe88324 = 1;
}
