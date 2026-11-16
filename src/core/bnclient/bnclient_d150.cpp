// 1.13c Bnclient+0xD150 -- stores 1 in the module global at image offset 0x1f614. The record is the global after the call.
#include <cstdint>
__attribute__((weak)) uint32_t g_bnclient_1f614;
void bnclient_d150() { g_bnclient_1f614 = 1u; }
