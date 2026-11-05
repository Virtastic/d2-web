// D2Game 0xF1C30: a point `step` units along the line from unit A to unit B, handed to the 0x75E10 walk.
// Convention (disassembly of the 1.13c bytes): EAX = A, EDI = B, stack game, step (ret 8). The distance of (B, A) (thunk 0xAB86, stdcall) comes
// first; 0 returns 0. A's position is read before that test, B's after: nx = ax + (bx - ax) * step / dist, ny likewise (32-bit products, signed idiv).
// Then thunk 0xA496(B, 0) runs and a 32-byte block {2, B, 0, nx, ny, 0x6500, 0, 0} goes to 0x75E10(game, &block, 1).
#include "d2game_s15.h"

// 1.13c D2Game+0xF1C30
extern "C" uint32_t d2game_f1c30(uint32_t a, uint32_t b, uint32_t game, uint32_t step, const D2S15* e)
{
    const D2Seam& s = *e->sm;
    const int32_t dist = (int32_t)s.call(0xAB86, 0, 0, 0, 0, 0, b, a);
    const uint32_t ax = s15_pos(s, a, false), ay = s15_pos(s, a, true);
    if (!dist) return 0;
    const uint32_t bx = s15_pos(s, b, false);
    const int32_t dx = (int32_t)((int64_t)(int32_t)((bx - ax) * step) / dist);
    const uint32_t by = s15_pos(s, b, true);
    const int32_t dy = (int32_t)((int64_t)(int32_t)((by - ay) * step) / dist);
    const uint32_t nx = ax + (uint32_t)dx, ny = (uint32_t)dy + ay;
    s.call(0xA496, 0, 0, 0, 0, 0, b, 0);
    uint32_t blk[8] = { 2, b, 0, nx, ny, 0x6500, 0, 0 };
    const uint32_t ret = s.call(0x75E10, 0, 0, 0, 0, 0, game, (uint32_t)(uintptr_t)blk, 1);
    e->blkm(e->ctx, 0x75E10, blk, 8, 0xFFFFFFFDu);
    return ret;
}
