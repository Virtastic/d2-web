// 1.13c D2Client+0x1E5F0 -- the loading screen frame. No args, plain ret. gl block:
// +0 the player slot 0x11BBFC, +4 the gate 0xFADBC, +0xC the pending flag 0xFADC4, +0x10 the version word 0xDD5D0, +0x14 0x1223D8,
// +0x18 the byte 0x1223F0, +0x1C the screen mode 0x11C414, +0x20 0x11C300, +0x24 0x11C304, +0x28 screen W 0xDBC48, +0x2C screen H
// 0xDBC4C, +0x30 0x11B824, +0x34 0x11B828, +0x40 the 0xF9E20 row table (stride 0x14; +0 x, +0xC cel y, +0x10 text y).
// No player, or the player's +0x10 is 0x11: Fog #10227; the version word must be 0x26 (else #10265(0x139B), #10024, _exit(-1));
// clear the pending flag and, if it was set, 0x3CC60(EAX 4, D2Gfx #10048(), 0xD07BC) and 0x1C720; then if 0x1223D8 is 1 clear it
// and the byte 0x1223F0. Else mode 3 or the gate ends it; row = 0x1C980(EAX mode); the 0x48-byte record {+0x34 = 0x11C304} gives
// the cel (#10021) and its width (#10060); string 0xF58 (D2Lang #10003) is centred on it with D2Win #10028/#10150 at
// (w/2 - tw/2 + row.x + 1, row.ty + H - 0x1E0); then two D2Gfx #10041 draws at (row.x, row.cy + H - 0x1E0) and (+3, -4), the
// second with +0x34 = 0x11C300 and frame +0 = (byte 0x1223F0 && low byte of 0x1D840(X 0x11B828, W 0x11B824)). Mode 2 adds
// (W - 0x280)/2 to every x. All halvings are signed and round toward zero.
#include <cstring>
#include "cl_env.h"
extern "C" void d2c_1e5f0(const cl::Env& e) {
    const uint32_t g = e.gl;
    const uint32_t pl = e.R(g + 0);
    if (!pl || e.R(pl + 0x10) == 0x11) {
        const uint32_t pend = e.R(g + 0xc);
        e.H(0xbf8c);
        if (e.R(g + 0x10) != 0x26) {
            const uint32_t m = e.H(0xbf92, 0x139b);
            e.H(0xbf80, 0xB20D359Fu, m);
            e.H(0x3664, 0xffffffffu);
        }
        e.W(g + 0xc, 0);
        if (pend) {
            const uint32_t hw = e.H(0xd0b4);
            e.H(0x3cc60, hw, 0xB20D07BCu, 0, 0, 0, 4);
            e.H(0x1c720);
        }
        if (e.R(g + 0x14) == 1) { e.W(g + 0x18, e.R(g + 0x18) & ~0xffu); e.W(g + 0x14, 0); }
        return;
    }
    const uint32_t mode = e.R(g + 0x1c);
    if (mode == 3 || e.R(g + 4)) return;
    uint32_t rec[18] = {}, snap[18];
    rec[13] = e.R(g + 0x24);
    const uint32_t p = (uint32_t)(uintptr_t)rec, sp = (uint32_t)(uintptr_t)snap;
    const uint32_t row = g + 0x40 + e.H(0x1c980, 0, 0, 0, 0, 0, mode) * 0x14u;
    memcpy(snap, rec, sizeof rec);
    const uint32_t cel = e.H(0xc0e8, p);
    if (e.blk) e.blk(e.ctx, 0xc0e8, sp, 18);
    const int32_t w = (int32_t)e.H(0xc0d6, cel);
    const uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xf58);
    const int32_t tw = (int32_t)e.H(0xd35a, 0, 0, 0, 0, 0, 0, s);
    const uint32_t H = e.R(g + 0x2c);
    const uint32_t dx = (uint32_t)((int32_t)(e.R(g + 0x28) - 0x280u) / 2);   // the mode-2 centring offset, signed halving
    uint32_t x = (uint32_t)(w / 2) - (uint32_t)(tw / 2) + e.R(row) + 1;
    const uint32_t ty = e.R(row + 0x10) + H - 0x1e0;
    if (e.R(g + 0x1c) == 2) x += dx;
    const uint32_t s2 = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xf58);
    e.H(0xd37e, ty, 0, 0, 0, 0, 0, s2, x);
    uint32_t cx = e.R(row);
    const uint32_t cy = e.R(row + 0xc) + H - 0x1e0;
    if (e.R(g + 0x1c) == 2) cx += dx;
    memcpy(snap, rec, sizeof rec);
    e.H(0xd096, p, cx, cy, 0xffffffffu, 0, 5, 0);
    if (e.blk) e.blk(e.ctx, 0xd096, sp, 18);
    rec[13] = e.R(g + 0x20);
    rec[0] = 0;
    if (e.R8(g + 0x18) && (e.H(0x1d840, e.R(g + 0x34), e.R(g + 0x30)) & 0xff)) rec[0] = 1;
    memcpy(snap, rec, sizeof rec);
    e.H(0xd096, p, cx + 3, cy - 4, 0xffffffffu, 0, 5, 0);
    if (e.blk) e.blk(e.ctx, 0xd096, sp, 18);
}
