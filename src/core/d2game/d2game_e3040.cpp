// 1.13c D2Game+0xE3040 -- stdcall, three arguments: clears the byte the third argument points at and returns with AL = 0; the upper bytes of EAX are those of that pointer.
#include <cstdint>
uint32_t d2game_e3040(uint32_t, uint32_t, uint8_t* out) { *out = 0; return (uint32_t)(uintptr_t)out & 0xFFFFFF00u; }
