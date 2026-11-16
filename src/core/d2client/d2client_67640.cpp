// 1.13c D2Client+0x67640 -- fastcall (ECX = a list control c; rows of 0x300E bytes from c+0x808, six word cells per row at +-0x800 steps):
// the scroll bar. Counts the rows (from the top row c+0x180730 to the row count c+0x180748) whose running height stays under c+0x180714-0x14
// (c+0x18074E), and when that is below the count draws the thumb: position and size from the track length c+0x180718, five draws of the
// 0xD096 cel call (stdcall 6; the 18-dword draw block is logged after each). Leaves position c+0x180752 and size c+0x180756.
#include "cl_pa4.h"
extern "C" void d2c_67640(uint32_t c, const cl::Env& e) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    const auto F = [&](uint32_t o) { return e.R(c + 0x180000u + o); };
    const auto SF = [&](uint32_t o, uint32_t v) { e.W(c + 0x180000u + o, v); };
    const int32_t top = (int32_t)F(0x730);
    const int32_t cnt = (int32_t)(F(0x748) & 0xffffu);
    SF(0x752, 0); SF(0x756, 0); SF(0x74e, 0);
    if (top < cnt) {
        uint32_t acc = 0;
        const int32_t lim = (int32_t)F(0x714) - 0x14;
        uint32_t row = (uint32_t)top * 0x300eu + 0x808u + c;
        for (int32_t i = top; i < (int32_t)(F(0x748) & 0xffffu); ++i, row += 0x300e) {
            static const uint32_t off[6] = {0xfffff800u, 0, 0x800, 0x1000, 0x1800, 0x2000};
            for (int k = 0; k < 6; ++k)
                if (e.R(row + off[k]) & 0xffffu) acc += F(0x766);
            acc += F(0x76a);
            if ((int32_t)acc < lim) SF(0x74e, F(0x74e) + 1);
        }
    }
    const int32_t k = (int32_t)F(0x74e);
    if (cnt <= k) return;
    const int32_t q = (k << 7) / cnt;
    const uint32_t M = F(0x718);
    int32_t pos = top;
    uint32_t size;
    if (top == 0) {
        size = (M * (uint32_t)q) >> 7;
    } else if (top < cnt - k) {
        const int32_t t = (int32_t)(M * (uint32_t)top) / cnt;
        pos = t < 0 ? 0 : t;
        const uint32_t s = ((M * (uint32_t)q) >> 7) + (uint32_t)pos;
        size = (int32_t)M < (int32_t)s ? M : s;
    } else {
        const uint32_t s = M - ((M * (uint32_t)q) >> 7);
        pos = (int32_t)M >= (int32_t)s ? (int32_t)s : (int32_t)M;
        size = M;
    }
    const uint32_t y0 = F(0x75e) + 0xd, x = F(0x75a) - 1;
    uint32_t b[18] = {};
    b[13] = pa4::G(e, 0x1154ec);
    b[0] = 0x10;
    const auto draw = [&](uint32_t y) {
        e.H(0xd096, P(b), x, y, 0xffffffffu, 5);
        if (e.blk) e.blk(e.ctx, 0xd096, P(b), 18);
    };
    draw(y0 + (uint32_t)pos * 12);
    b[0] = 0xe;
    uint32_t last = (uint32_t)pos;
    if (pos < (int32_t)size) {
        uint32_t y = y0 + (uint32_t)pos * 12 + 0xc;
        for (int32_t n = (int32_t)size - pos; n; --n, y += 12) draw(y);
        last = size;
    }
    b[0] = 0xf;
    draw(y0 + (last * 3 + 3) * 4);
    SF(0x752, (uint32_t)pos);
    SF(0x756, size);
}
