// 1.13c D2Launch+0x12a60 -- stores 1 in the module global at image offset 0x25e60.
#include <cstdint>
__attribute__((weak)) uint32_t g_d2launch_25e60;
void d2launch_12a60() { g_d2launch_25e60 = 1; }
