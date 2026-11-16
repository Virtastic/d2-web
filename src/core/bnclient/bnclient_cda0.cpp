// 1.13c Bnclient+0xCDA0 -- returns the module global at image offset 0x1f508.
#include <cstdint>
__attribute__((weak)) uint32_t g_bnclient_1f508;
uint32_t bnclient_cda0() { return g_bnclient_1f508; }
