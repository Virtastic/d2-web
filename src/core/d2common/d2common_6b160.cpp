// 1.13c D2Common+0x6B160 -- callback relay (ecx = the item, edx = the argument block): a null block answers 0; otherwise the worker 0x6A790 is called with the block words 0 and 1, the item, and the block words 2 and 3, and its answer is passed on.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f6b160(GE& e, uint32_t it, uint32_t blk)
{
    if (!blk) return 0;
    const uint32_t r = e.h(0x6a790, 0, 0, 0, 0, 0, e.rd32(blk), e.rd32(blk + 4), it, e.rd32(blk + 8));
    const uint32_t w[1] = { e.rd32(blk + 12) };
    e.x(0x6a790, w, 1);
    return r;
}
}
