// Line-cache step: rebuild one cell of the line cache from its neighbour(s) along the current step mode. A neighbour
// takes its override when that is nonzero; the blend modes mix two neighbours by the stored weight (8-bit fraction).
#include "d2client_light_env.h"
using namespace cllight;
static uint32_t pick(const cl::Env& e, int32_t i) {
    const uint32_t v = e.R(kLineB + (uint32_t)i * 4u);
    return v ? v : e.R(kLineA + (uint32_t)i * 4u);
}
extern "C" void d2c_a8310(int32_t row, int32_t col, const cl::Env& e) {
    const uint32_t mode = e.R(kMode);
    if (mode > 4 || mode == 0) return;
    const int32_t dx = (int32_t)e.R(kDx), sg = (int32_t)e.R(kSign);
    const int32_t here = row * 64 + col, next = (row + sg) * 64 + col;
    uint32_t v;
    if (mode == 1) v = pick(e, here + dx);
    else if (mode == 4) v = pick(e, next);
    else {
        const uint32_t a = pick(e, mode == 2 ? here + dx : next), b = pick(e, next + dx), f = e.R(kFrac);
        v = (uint32_t)((int32_t)((0x100u - f) * a + f * b) >> 8);
    }
    e.W(kLineA + (uint32_t)here * 4u, v);
}
