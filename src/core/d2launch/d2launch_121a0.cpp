// 1.13c D2Launch+0x121a0 -- stores 0 in the module global at image offset 0x25ad0.
#include <cstdint>
__attribute__((weak)) uint32_t g_d2launch_25ad0;
void d2launch_121a0() { g_d2launch_25ad0 = 0; }
