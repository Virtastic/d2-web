// 1.13c D2Client+0x14FD0: select the unit's list entry for a table row and snap its path to the found target.
#include "cl_k.h"
#include <emscripten.h>
using namespace clk;
extern "C" uint32_t d2c_14fd0(uint32_t unit, int32_t idx, const Env& e) {
    if (!row(e, idx)) return 0;
    const uint32_t lx = e.H(0xc310, unit);
    if (!lx) return 0;
    const uint32_t v = e.H(0xcb9e, lx);
    const uint32_t r = e.H(0xcb6e, lx);
    const uint32_t rec = e.H(0xa4e20, r, 0, 0, 0, 0, 0, 0, v & 0x7f, v, 0xB2000000u | (0x10A608u + (r << 9)));
    const uint32_t ebp = e.H(0xc700, unit) + 4;
    uint32_t o = rec;
    Out out;
    if (!(rec && e.H(0xc6e2, unit, rec, 0))) {
        out = { { 0, 0xffffffffu, 0, v, v, 0 } };
        e.H(0xa1860, unit, 0, 0, ebp, 0x2a283, 0xB20A09F0u, (uint32_t)(uintptr_t)&out);
        o = out.w[0] ? out.w[0] : out.w[2];
        if (!o) return 0;
    }
    const int32_t n = (int32_t)e.H(0xcb68, lx) - 1;
    e.H(0xcb50, lx, (uint32_t)n);
    if (n <= 0) return 1;
    const uint32_t x = o ? e.R(o + 0xc) : 0xffffffffu;
    out = { { 0, 0xffffffffu, 0, x, x, 0 } };
    e.H(0xa1860, unit, 0, 0, ebp, 0x2a283, 0xB20A09F0u, (uint32_t)(uintptr_t)&out);
    o = out.w[0] ? out.w[0] : out.w[2];
    if (!o) return 0;
    const uint32_t Y = e.H(0x1660, 0, 0, 0, 0, 0, 0, 0, 0, o);
    const uint32_t X = e.H(0x1630, 0, 0, 0, 0, 0, 0, 0, 0, o);
    e.H(0xc478, e.R(unit + 0x2c), X, Y, 0);
    e.H(0xcb7a, lx, e.R(o));
    e.H(0xcb8c, lx, e.R(o + 0xc));
    e.H(0xc42a, lx, 0xB20D6FE8u, 0x250);
    e.H(0x2be0);
    e.H(0xa05f0);
    e.H(0xc7a8, 0, 0, 0, 0, 0, 0, 0, 0, unit);
    e.H(0x22280, unit, 0, 0, 0, 0, 0, 0, 0, unit);
    e.H(0x88a70);
    return 1;
}
