// 1.13c Bnclient+0xE920 -- returns the module global at image offset 0x1f668.
#include <cstdint>
__attribute__((weak)) uint32_t g_bnclient_1f668;
uint32_t bnclient_e920() { return g_bnclient_1f668; }
