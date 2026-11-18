// Add light to the cell under world (x, y): level += amount (capped at 255). With coloured lighting each channel becomes
// (old * old level + new * amount) scaled by the curve at the new level (>> 16, capped at 255); without it the channels clear.
#include "d2client_light_env.h"
using namespace cllight;
extern "C" void d2c_a8a40(int32_t x, int32_t y, int32_t amt, uint32_t r, uint32_t g, uint32_t b, const cl::Env& e) {
    const int32_t cx = (x >> 3) - (int32_t)e.R(kOrgX);
    if (cx < 0 || cx >= 0x30) return;
    const int32_t cy = (y >> 3) - (int32_t)e.R(kOrgY);
    if (cy < 0 || cy >= 0x30) return;
    const uint32_t p = cell(cx, cy), lv = r8(e, p + 4);
    int32_t nl = (int32_t)lv + amt; if (nl >= 0xFF) nl = 0xFF;
    uint32_t c0 = 0, c1 = 0, c2 = 0;
    if (e.R(kColour)) {
        const uint32_t k = e.R(kCurve + (uint32_t)nl * 4u);
        auto ch = [&](uint32_t old, uint32_t nw) { uint32_t v = ((old * lv + (nw & 0xFFu) * (uint32_t)amt) * k) >> 16; return v > 0xFF ? 0xFFu : v; };
        c0 = ch(r8(e, p + 5), r); c1 = ch(r8(e, p + 6), g); c2 = ch(r8(e, p + 7), b);
    }
    e.W(p + 4, ((uint32_t)nl & 0xFFu) | c0 << 8 | c1 << 16 | c2 << 24);
}
