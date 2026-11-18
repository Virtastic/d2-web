// Blocker mask: for the light at rec (x +0x10, y +0x14, radius +0x18), write (blocker byte << 4) for each cell of the
// (2r/8 + 1)-square around it into the override lines, centred on line row/col 32; a cell outside the grid counts as blocker 1.
#include "d2client_light_env.h"
using namespace cllight;
extern "C" void d2c_a8950(uint32_t rec, const cl::Env& e) {
    const int32_t x = (int32_t)e.R(rec + 0x10), y = (int32_t)e.R(rec + 0x14), rad = (int32_t)e.R(rec + 0x18);
    const int32_t x0 = (x - (x & 7)) - rad, y0 = (y - (y & 7)) - rad, n = (int32_t)((uint32_t)rad * 2u) >> 3;
    if (n < 0) return;
    uint32_t line = kLineB + (uint32_t)(0x20 - (rad >> 3)) * 0x104u;
    int32_t wy = y0;
    for (int32_t j = 0; j <= n; ++j, line += 0x100, wy += 8) {
        int32_t wx = x0; uint32_t p = line;
        for (int32_t i = 0; i <= n; ++i, wx += 8, p += 4) {
            const int32_t cx = (wx >> 3) - (int32_t)e.R(kOrgX), cy = (wy >> 3) - (int32_t)e.R(kOrgY);
            const uint32_t bl = (cx < 0 || cx >= 0x30 || cy < 0 || cy >= 0x30) ? 1u : r8(e, cell(cx, cy));
            e.W(p, bl << 4);
        }
    }
}
