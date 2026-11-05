// Light sample: world (x, y) to a grid cell (clamped to the 48 x 48 window), its level into *lv and, with coloured
// lighting on, its three channels into *r, *g, *b.
#include "d2client_light_env.h"
using namespace cllight;
extern "C" void d2c_light_a8230(int32_t x, int32_t y, uint32_t lv, uint32_t r, uint32_t g, uint32_t b, const cl::Env& e) {
    int32_t cx = (x >> 3) - (int32_t)e.R(kOrgX), cy = (y >> 3) - (int32_t)e.R(kOrgY);
    cx = cx < 0 ? 0 : cx > 0x2f ? 0x2f : cx;
    cy = cy < 0 ? 0 : cy > 0x2f ? 0x2f : cy;
    const uint32_t c = cell(cx, cy);
    w8(e, lv, r8(e, c + 4));
    if (!e.R(kColour)) return;
    w8(e, r, r8(e, c + 5));
    w8(e, g, r8(e, c + 6));
    w8(e, b, r8(e, c + 7));
}
