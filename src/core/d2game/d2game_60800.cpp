// 1.13c D2Game+0x60800 -- stdcall, three arguments, all ignored: returns with AL = 0; the upper three bytes of EAX are the caller's.
#include <cstdint>
uint32_t d2game_60800(uint32_t eax, uint32_t, uint32_t, uint32_t) { return eax & 0xFFFFFF00u; }
