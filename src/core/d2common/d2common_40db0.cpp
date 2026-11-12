// 1.13c D2Common+0x40DB0 -- cell test (ecx = the grid owner, edx = x; stack: y, wanted kind, unused, packed cell word): asks the worker 0x40C20 for the kind of the packed word (ecx = owner, edx = bits 20..25, stack = bits 8..15); -5 answers 1, any kind other than the wanted one answers 0, otherwise the answer is the inverse of bit 10 of the grid cell at (x, y) of the grid at +0x2C of the owner block at +0x14 (row offsets at +0x30).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f40db0(GE& e, uint32_t a, uint32_t x, uint32_t y, uint32_t want, uint32_t un, uint32_t w)
{
    (void)un;
    const uint32_t v = e.h(0x40c20, 0, a, (w >> 20) & 0x3f, 0, 0, (w >> 8) & 0xff);
    if (v == 0xfffffffbu) return 1;
    if (v != want) return 0;
    const uint32_t g = e.rd32(a + 0x14);
    return (~(e.rd32(e.rd32(g + 0x2c) + (e.rd32(e.rd32(g + 0x30) + y * 4) + x) * 4) >> 10)) & 1;
}
}
