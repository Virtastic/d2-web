// 1.13c D2Multi+0x84B0 -- stdcall, one ignored argument: stores 1 in the module global at image offset 0x39118 and returns 1.
#include <cstdint>
__attribute__((weak)) uint32_t g_d2multi_39118;
uint32_t d2multi_84b0(uint32_t) { g_d2multi_39118 = 1; return 1; }
