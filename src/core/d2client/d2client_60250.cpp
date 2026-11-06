// 1.13c D2Client+0x60250: automap cell-tree walk, ECX = node, EDX = rect {cel, x0, x1, y0, y1, ox, oy}. Globals ride in the pinned dword array e.gl:
// 0 11C414 side, 1 DBC48 gW, 2 F16B0 scale, 3 11C1F8 scrX, 4 11C1FC scrY, 5 11BBFC player, 6 11C1B0 mode, 7 11C1C8 cel0, 8 F9E18, 9 F9E14, 10 DBC4C gH,
// 11 11C240 panel, 12 11C284 act. Node: +4 s16 cel frame, +6/+8 s16 world x/y, +0xC left subtree, +0x10 next. Screen x/y = pos*10/scale - scroll.
// 0x5F9C0() (register-free, reads/writes 11C208 itself) returns the cell kind; it is called once per drawn node and once more when the kind is 1 and the
// node passes the iso-distance box; the "hi" the old port named is retail's EDX = distance/2 at that second call. The clip rect r4 and the 18-dword
// request rec are adjacent stack locals (r4 first), handed to 0xD162 (Ordinal_10079, stdcall, 5 words: &rec, x, y, &r4, col). `lm` picks what the
// probe logs after the call record: 0 = the 4-dword clip rect (stack word 3), 1 = the 18-dword request (stack word 0).
#include "cl_env.h"
namespace {
struct Blk { int32_t r4[4]; int32_t rec[18]; };
void walk(const cl::Env& e, uint32_t node, uint32_t rc, uint32_t lm) {
    auto G = [&](int i) { return (int32_t)e.R(e.gl + 4u * (uint32_t)i); };
    while (node) {
        const int32_t gW = G(1);
        const int32_t q = gW + ((gW >> 31) & 3);
        const int32_t side = G(0);
        const int32_t sh = side == 1 ? -(q >> 2) : side == 2 ? (q >> 2) : 0;
        const int32_t scale = G(2);
        const int32_t x = (e.R16s(node + 6) * 10) / scale - G(3);
        const int32_t y = (e.R16s(node + 8) * 10) / scale - G(4);
        if ((int32_t)e.R(rc + 0xc) <= y) {
            walk(e, e.R(node + 0xc), rc, lm);
            if ((int32_t)e.R(rc + 0xc) <= y) {
                if (y > (int32_t)e.R(rc + 0x10)) return;
                if (x >= (int32_t)e.R(rc + 4) && x <= (int32_t)e.R(rc + 8)) {
                    Blk b = {};
                    b.rec[0] = e.R16s(node + 4);
                    b.rec[13] = (int32_t)e.R(rc);
                    int32_t col = 5;
                    const uint32_t k = e.H(0x5f9c0);
                    const int32_t mode = G(6);
                    if (k == 1) {
                        if ((int32_t)e.R(rc) == G(7) && mode == 0) {
                            const int32_t px = (int32_t)e.R(rc + 0x14) + x, py = (int32_t)e.R(rc + 0x18) + y;
                            const int32_t cy = G(8) / 2, cx = G(9) / 2;
                            if (px >= cx + sh - 0x8c && px <= cx + sh + 0x8c && py >= cy - 0x96 && py <= cy + 0x82) {
                                int32_t dx = (cx - px) + sh, dy = (cy - 10) - py;
                                if (dx < 0) dx = -dx;
                                if (dy < 0) dy = -dy;
                                const int32_t d = dx <= dy ? dx + dy * 2 : dy + dx * 2;
                                const int32_t hi = d / 2;
                                if (hi < 0x96) {
                                    const uint32_t lo = e.H(0x5f9c0);
                                    col = (lo != 2) + 1;
                                    if (hi < 0x64) { col = 1; if (hi < 0x32) col = 0; }
                                }
                            }
                        }
                    } else if (k == 2) {
                        col = 1;
                    } else if (k == 3) {
                        const uint32_t st = e.R(e.R(e.gl + 4 * 5) + 0x18) & 0xff;
                        if (st == 0 || st == 2) col = mode == 1 ? 1 : 2;
                    }
                    if (G(6) == 1) {
                        int32_t a, c;
                        if (G(11) == 0) { a = G(1) - 0x119; c = 0x39; }
                        else { a = 0; c = (((G(12) == 2) - 1) & 0x60) - 0x15; }
                        b.r4[0] = a; b.r4[1] = a + 0x117; b.r4[2] = c; b.r4[3] = c + 0xe1;
                    } else {
                        b.r4[0] = 0; b.r4[1] = G(1) - 1; b.r4[2] = 0; b.r4[3] = G(10) - 1;
                    }
                    const uint32_t pr4 = (uint32_t)(uintptr_t)b.r4, prec = (uint32_t)(uintptr_t)b.rec;
                    e.H(0xd162, prec, (uint32_t)x, (uint32_t)y, pr4, (uint32_t)col);
                    if (e.blk) { if (lm == 0) e.blk(e.ctx, 0xd162, pr4, 4); else e.blk(e.ctx, 0xd162, prec, 18); }
                }
            }
        }
        if (y > (int32_t)e.R(rc + 0x10)) return;
        node = e.R(node + 0x10);
    }
}
}  // namespace
extern "C" void d2c_60250e(uint32_t node, uint32_t rc, const cl::Env& e, uint32_t lm) { walk(e, node, rc, lm); }
