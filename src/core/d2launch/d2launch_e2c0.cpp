// 1.13c D2Launch+0xe2c0 -- stores 1 in the module global at image offset 0x25ea0.
#include <cstdint>
__attribute__((weak)) uint32_t g_d2launch_25ea0;
void d2launch_e2c0() { g_d2launch_25ea0 = 1; }
