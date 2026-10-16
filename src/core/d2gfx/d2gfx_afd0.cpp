// 1.13c D2Gfx+0xAFD0: forward to the loaded renderer's slot +0x90 with (ecx = a, edx = b + 2, stack = c).
#include <cstdint>
typedef uint32_t (*D2GfxSlot90)(void* backEnd, uint32_t ecx, uint32_t edx, uint32_t arg);
extern "C" uint32_t d2gfx_sub_afd0(uint32_t a, uint32_t b, uint32_t c, D2GfxSlot90 slot, void* backEnd) { return slot(backEnd, a, b + 2, c); }
