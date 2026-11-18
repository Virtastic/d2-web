// src/core/d2cmp/d2cmp_e460.cpp -- 1.13c D2CMP sub_E460 (export 10034), transcribed from the cached decompile.
#include <cstdint>

// Toggle a module flag: flag := (flag == 0). The flag is DAT_6fe87efc in retail; the caller passes its storage.
// 1.13c D2CMP+0x0E460
void d2cmp_e460(uint32_t* flag) { *flag = (*flag == 0) ? 1u : 0u; }
