// 1.13c D2Client+0x750B0 (245 B, void, no arguments): the four-frame HUD cel.
// One zeroed 0x48-byte frame block on the stack, its cel word (+0x34) = g11BFDC; frames 0..3 (+0x00) drawn through D2Gfx 10041
// (thunk 0xD096, slot 0xCEBF4) at (x, yTop) (x+0x100, yTop) (x, yBot) (x+0x100, yBot), args (block, x, y, -1, 5, 0), with
// x = g11B9A0, yTop = g11B9A4 + gDBC4C - 0xE0, yBot = g11B9A4 + gDBC4C - 0x30; every global is re-read before each call.
// pos: the pinned 0x11B9A0 pair (x, y); cel: 0x11BFDC; scr: 0xDBC48 (W, H).
#include <cstdint>
#include "cl_env.h"
extern "C" void d2c_750b0(uint32_t pos, uint32_t cel, uint32_t scr, const cl::Env& e) {
    volatile uint32_t f[18] = {};
    f[13] = e.R(cel);
    for (uint32_t k = 0; k < 4; ++k) {
        f[0] = k;
        uint32_t x = e.R(pos) + ((k & 1) ? 0x100u : 0u);
        uint32_t y = e.R(pos + 4) + e.R(scr + 4) - ((k & 2) ? 0x30u : 0xe0u);
        uint32_t p = (uint32_t)(uintptr_t)f;
        e.Xs(0);
        e.H(0xCEBF4, p, x, y, 0xffffffffu, 5);
        if (e.blk) e.blk(e.ctx, 0xCEBF4, p, 18);
    }
}
