// 1.13c D2Win+0x10d80 -- fastcall(ecx = ctl): type-11 (list box) mouse hit test. Flags (+8) must carry gdwBitMasks[2] and [0];
// the row under the pointer, (y - top + scroll)/0x14, goes to +0x464 (-1 when the pointer is outside the box).
#include <cstdint>
#include <cstdlib>
extern "C" uint32_t d2win_10d80(uint32_t* c, int32_t mx, int32_t my)
{
    if (c[0] != 0xb) abort();
    if (!((4u & c[2]) && (1u & c[2]))) return 0;
    c[0x119] = 0xFFFFFFFFu;
    const int32_t dx = mx - (int32_t)c[3];
    const int32_t row = ((int32_t)c[6] - (int32_t)c[4]) + my;
    if (dx >= 0 && dx <= (int32_t)c[5] && row >= 0 && row <= (int32_t)c[6]) c[0x119] = (uint32_t)(row / 0x14);
    return 1;
}
