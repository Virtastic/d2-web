// 1.13c D2Common+0x36BE0 -- ECX = slot, EDX = value: reads the slot; an empty slot takes the value, a filled one is stored back unchanged. Returns the slot value read.
#include <cstdint>
uint32_t d2common_36be0(uint32_t* slot, uint32_t value) { const uint32_t cur = *slot; *slot = cur ? cur : value; return cur; }
