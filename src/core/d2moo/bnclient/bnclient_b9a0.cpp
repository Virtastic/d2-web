// 1.13c Bnclient+0xB9A0 -- ECX = out: stores the module global at image offset 0x1EF48 through it and returns the global at 0x1EDC8.
#include <cstdint>
__attribute__((weak)) uint32_t g_bnclient_1ef48;
__attribute__((weak)) uint32_t g_bnclient_1edc8;
uint32_t bnclient_b9a0(uint32_t* out) { *out = g_bnclient_1ef48; return g_bnclient_1edc8; }
