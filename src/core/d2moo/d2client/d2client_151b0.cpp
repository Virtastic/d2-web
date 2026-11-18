// 1.13c D2Client+0x151B0 and 0x15350: refresh a unit's list selection for a table row and push the selected unit's position
// The two differ only in the selection search and the min seed.
#include "cl_k.h"
#include <emscripten.h>
using namespace clk;
// shared tail: the selection's x/y into the path, then the selected type and seed
static void tail(const Env& e, uint32_t unit, uint32_t lx, uint32_t sel) {
    const uint32_t ty = e.R(sel), sp = e.R(sel + 0x2c);
    uint32_t Y, X;
    if (statPath(ty)) Y = e.R(sp + 0x10); else Y = sp ? e.H(0xc2ec, sp) : 0;
    if (statPath(ty)) X = e.R(sp + 0xc); else X = sp ? e.H(0xc3be, sp) : 0;
    e.H(0xc478, e.R(unit + 0x2c), X, Y, 0);
    e.H(0xcb7a, lx, ty);
    e.H(0xcb8c, lx, e.R(sel + 0xc));
}
extern "C" uint32_t d2c_151b0(uint32_t unit, int32_t idx, uint32_t sA, const Env& e) {
    const uint32_t rw = row(e, idx);
    if (!rw) return 0;
    const uint32_t lx = e.H(0xc310, unit);
    if (!lx) return 0;
    const uint32_t ebp = e.H(0xc700, unit) + 4;
    e.H(0x2a0b0, 0, 0, 0, 0, 0, 0, 0, unit);
    const uint32_t path = e.R(unit + 0x2c);
    if (!path) return 0;   // retail's fatal-exit branch (0xBF92, 0xBF80, 0x3664): the capture never reaches it
    uint32_t esi = e.H(0xc2da, path);
    if (!esi) {
        Out out = { { 0, 0xffffffffu, 0, 0xffffffffu, 0xffffffffu, 0 } };
        e.H(0xa1860, unit, 0, 0, ebp, 0x2a283, 0xB20A09F0u, (uint32_t)(uintptr_t)&out);
        esi = out.w[0] ? out.w[0] : out.w[2];
        if (!esi) { e.H(0xcb50, lx, 0); return 1; }
    }
    e.H(0xcb50, lx, e.H(0xc316, unit, e.R(rw + 0x138), (uint32_t)idx, sA));
    tail(e, unit, lx, esi);
    return 1;
}
extern "C" uint32_t d2c_15350(uint32_t unit, int32_t idx, uint32_t sA, const Env& e, void (*a1a50out)(void*, uint32_t*)) {
    const uint32_t rw = row(e, idx);
    if (!rw) return 0;
    const uint32_t lx = e.H(0xc310, unit);
    if (!lx) return 0;
    const uint32_t ebp = e.H(0xc700, unit) + 4;
    e.H(0x2a0b0, 0, 0, 0, 0, 0, 0, 0, unit);
    const uint32_t path = e.R(unit + 0x2c);
    if (!path) return 0;   // retail's fatal-exit branch: never reached by the capture
    uint32_t esi = e.H(0xc2da, path);
    uint32_t l10 = 0;
    if (!esi) {
        esi = e.H(0xa1a50, unit, 0, 0, 0, 0, 0, 0, 0x20003, 0xffffffffu, ebp);
        a1a50out(e.ctx, &l10);
        if (!esi) { e.H(0xcb50, lx, 0); return 1; }
    } else {
        e.H(0xa1860, unit, 0, 0, ebp, 0x2a783, 0xB20A0740u, (uint32_t)(uintptr_t)&l10);
    }
    const int32_t c = (int32_t)e.H(0xc316, unit, e.R(rw + 0x138), (uint32_t)idx, sA);
    e.H(0xcb50, lx, (uint32_t)((int32_t)l10 < c ? (int32_t)l10 : c));
    tail(e, unit, lx, esi);
    return 1;
}
