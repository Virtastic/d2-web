// 1.13c D2Multi+0x90B0 -- stdcall, one ignored argument: stores 1 in the module global at image offset 0x3a09c and returns 1.
#include <cstdint>
__attribute__((weak)) uint32_t g_d2multi_3a09c;
uint32_t d2multi_90b0(uint32_t) { g_d2multi_3a09c = 1; return 1; }
