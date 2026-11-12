// 1.13c D2Game+0x2A0C0 -- ECX = value, EDX = slot: an empty slot takes the value; a filled one is left. EAX is not touched.
#include <cstdint>
void d2game_2a0c0(uint32_t value, uint32_t* slot) { if (*slot == 0) *slot = value; }
