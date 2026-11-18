// 1.13c Bnclient+0xE930 -- sets the low byte of EAX to 0x3C and returns; the upper three bytes are the caller's.
#include <cstdint>
uint32_t bnclient_e930(uint32_t eax) { return (eax & 0xFFFFFF00u) | 0x3Cu; }
