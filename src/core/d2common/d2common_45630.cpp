// 1.13c D2Common+0x45630 -- returns the constant 1 and drops its 7 stack arguments; reads and writes nothing.
#include <cstdint>
uint32_t d2common_45630(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) { return 1u; }
