// 1.13c D2Client+0x861A0 itemPkt_ownerSwapHand: stack arg = packet (ret 4). g = address of the retail 0x11BF58 block (words at +0 and +4).
#include "cl_env.h"
extern "C" void d2c_861a0(uint32_t pkt, uint32_t g, const cl::Env& e) {
    const uint32_t path = 0xB2000000u | 0xD4964;
    uint32_t B[8] = {};
    const uint32_t bp = (uint32_t)(uintptr_t)B;
    e.H(0xca90, pkt + 0xd, e.R8(pkt + 2) - 0xd, 0, bp);
    const uint32_t P = e.self;
    const uint32_t id = e.R(pkt + 9), bk = e.R8(pkt + 8);
    const uint32_t u = e.H(0xa4e20, bk, 0, 0, 0, 0, id & 0x7f, id, 0xB2000000u | (0x10a608 + bk * 0x200));
    if (!u) return;
    if (!(e.R(u + 0xc4) & 0x10000)) {
        uint32_t t = e.R(u), m = e.R(u + 0x10);
        if (t == 0 && m != 0 && m != 0x11) e.H(0x19830, 0, 1, 0, 0, 0, 0, 7, u);
    }
    const uint32_t ty = e.R8(bp + 0x11);
    uint32_t md;
    if (ty == 4 || ty == 0xb) md = 5; else if (ty == 5 || ty == 0xc) md = 4; else return;
    const uint32_t inv = e.R(u + 0x60);
    uint32_t X = e.H(0xc262, inv, md);
    if (!X) return;                                             // fatal 0x5EF in retail
    e.H(0xca2a, inv, X);
    uint32_t al = e.H(0xc9f4, u, P) & 0xff;
    e.H(0x69a90, 0, 0, 0, 0, 0, al, u);
    e.H(0xc9ca, u, inv, (uint32_t)(uintptr_t)&X, md);
    uint32_t r = e.H(0xc6d0, inv, X);
    if (!r || r != X) return;                                   // fatal 0x5F7 in retail
    if (e.R(u) == 0) e.H(0xca0c, e.R(u + 0x60));
    if (e.H(0xc1a8, r, 0x100, 0x96, path) == 0) e.H(0xc544, u, r);
    e.H(0xcaa8, inv, md);
    e.H(0x6a220, 0, 0, 0, 0, 0, u);
    if (u == P) {
        e.H(0x8bbf0);
        if (ty == 4) e.W(g, e.R(g) & 0xffff0000u);
        else if (ty == 5) e.W(g + 4, e.R(g + 4) & 0xffff0000u);
        e.H(0x82980, 0, 4, 0, e.R(bp + 0x10), 0, X);
        e.H(0xc9dc, X, 0);
        e.H(0xc7d8, inv, X);
        uint32_t c = e.H(0xc202, inv);
        e.H(0x162d0, 0, 0, 0, 0, 0, c);
    }
    e.H(0x81aa0, 0, 0, 0, 0, 0, 0, 0, 0, 0, X);
    const uint32_t id2 = e.R(pkt + 4);
    uint32_t Y = 0;
    for (uint32_t n = e.R(e.gl + (id2 & 0x7f) * 4); n; n = e.R(n + 0xe4))
        if (e.R(n + 0xc) == id2) {
            if (e.R(n) != 4) return;                            // fatal 0x5A in retail
            Y = n;
            break;
        }
    if (Y) {
        if (e.H(0xca3c, e.R(bp + 0xc), 1)) {
            e.H(0xa6ef0, id2, 4, 0, 0, 0, 0, 0xB2000000u | 0x10ae08, id2 & 0x7f);
            Y = e.H(0x833f0, pkt, bp, 0, 0, 0, 0, 0, 0, 0);
            if (e.blk) e.blk(e.ctx, 0x833f0, bp, 6);
            if (!Y) return;                                     // fatal 0x618 in retail
        } else {
            e.H(0x82980, 0, e.R8(bp + 8), 0, e.R(bp + 0x10), 0, Y);
        }
    } else {
        Y = e.H(0x833f0, pkt, bp, 0, 0, 0, 0, 0, 0, 0);
        if (e.blk) e.blk(e.ctx, 0x833f0, bp, 6);
    }
    e.H(0xc9dc, Y, e.R(bp + 0x11));
    if (!e.H(0xca6c, inv, Y, (ty == 0xb || ty == 0xc) ? 4 : 3)) return;
    if (!e.H(0x81b40, 0, 0, 0, 0, 0, 0, 0, 0, Y, ty)) return;
    e.H(0xca18, inv, Y);
    e.H(0x68cb0, 0, 0, 0, 0, 0, u);
    int32_t ut = (int32_t)e.R(u);
    if (ut < 0 || ut > 1) return;                               // fatal 0x641 in retail
    uint32_t b3 = e.R8(pkt + 3);
    if (b3 == 0 || b3 == 1) e.H(0x69a90, 0, 0, 0, 0, 0, b3, u);
    e.H(0x6a220, 0, 0, 0, 0, 0, u);
    e.H(0x81b70, 0, 0, 0, 0, 0, Y);
    e.H(0x81ff0, 0);
    e.Ej(0x81ff0, u);
    e.H(0x82fe0, u);
    e.H(0x82fe0, u);
    e.H(0xbe2b0);
    e.H(0x81aa0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Y);
}
