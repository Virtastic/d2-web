// Step mode from the light at obj to (x, y): column and row steps take the delta's signs; mode 0 when both deltas are 0,
// 4 for a pure row step, 1 for a pure column step, else 2 (column major) or 3 (row major) with the 8-bit minor/major ratio.
#include "d2client_light_env.h"
using namespace cllight;
extern "C" void d2c_a8470(uint32_t obj, int32_t x, int32_t y, const cl::Env& e) {
    int32_t dx = (int32_t)e.R(obj + 0x10) - x, dy = (int32_t)e.R(obj + 0x14) - y;
    if (!dx && !dy) { e.W(kMode, 0); return; }
    if (dx < 0) { e.W(kDx, 0xFFFFFFFFu); dx = -dx; } else e.W(kDx, 1);
    if (dy < 0) { e.W(kSign, 0xFFFFFFFFu); dy = -dy; } else e.W(kSign, 1);
    if (!dx) { e.W(kMode, 4); return; }
    if (!dy) { e.W(kMode, 1); return; }
    if (dx >= dy) { e.W(kMode, 2); e.W(kFrac, (uint32_t)((int32_t)((uint32_t)dy << 8) / dx)); }
    else { e.W(kMode, 3); e.W(kFrac, (uint32_t)((int32_t)((uint32_t)dx << 8) / dy)); }
}
