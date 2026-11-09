// 1.13c D2Client+0xbaef0 -- missile effect spawn with no slot: an area scan through 0xA1860. ECX = unit, always returns 1.
// u == 0, Missiles row invalid, or s16 row+0x2C < 0: 1. c = 10240(u) (0xC388), m = 11029(u) (0xC3FA); c indexes the 0x23C-byte
// skill table at [dt+0xB98] (count [dt+0xBA0]); out of range: 1. p = 0xA60D0(EAX = u), 0: 1. X = 0x1630(ECX = u), Y = 0x1660(ECX = u).
// n1 = row+0x6C, when <= 0: 10786(p, sk+0x64, c, m) (0xC316) clamped to >= 1; n2 = row+0x70, when <= 0: 10786(p, sk+0x138, c, m) clamped.
// id = sk+0x50, 0 becomes 0xA783. A 6-dword stack context {p, u, c, m, n2, s16 row+0x2C} goes as the callback data of
// 0xA1860(p, X, Y, n1, id, 0xB6480 (the per-unit callback), &ctx, 1) (ret 0x20).
// The capture logs the call by `hookblk 0xa1860 k 23 0x7E00FF`: the 8 stack words, the caller's 9 saved/local words zeroed, then ctx.
#include <cstdint>
#include "cl_env.h"
extern "C" uint32_t d2c_baef0(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x2c) < 0) return 1;
    uint32_t c = e.H(0xc388, u), m = e.H(0xc3fa, u);
    uint32_t sk = 0;
    if ((int32_t)c >= 0 && (int32_t)c < (int32_t)e.R(e.dt + 0xba0)) sk = e.R(e.dt + 0xb98) + c * 0x23c;
    if (!sk) return 1;
    uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!p) return 1;
    uint32_t X = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    uint32_t Y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    int32_t n1 = (int32_t)e.R(row + 0x6c);
    if (n1 <= 0) { n1 = (int32_t)e.H(0xc316, p, e.R(sk + 0x64), c, m); if (n1 < 1) n1 = 1; }
    int32_t n2 = (int32_t)e.R(row + 0x70);
    if (n2 <= 0) { n2 = (int32_t)e.H(0xc316, p, e.R(sk + 0x138), c, m); if (n2 < 1) n2 = 1; }
    uint32_t id = e.R(sk + 0x50);
    if (!id) id = 0xa783;
    uint32_t w[23] = {};
    uint32_t* ctx = w + 17;
    ctx[0] = p; ctx[1] = u; ctx[2] = c; ctx[3] = m; ctx[4] = (uint32_t)n2; ctx[5] = (uint32_t)e.R16s(row + 0x2c);
    w[0] = p; w[1] = X; w[2] = Y; w[3] = (uint32_t)n1; w[4] = id; w[5] = 0xB2000000u | 0xb6480u; w[6] = (uint32_t)(uintptr_t)ctx; w[7] = 1;
    e.H(0xa1860, w[0], w[1], w[2], w[3], w[4]);
    if (e.blk) e.blk(e.ctx, 0xa1860, (uint32_t)(uintptr_t)w, 23);
    return 1;
}
