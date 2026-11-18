// 1.13c D2Client+0x84F00 itemPkt_ownerSlotUpdate: stack arg = packet (ret 4), returns the (re)created item or 0.
// g58 = address of the retail 0x11BF58 block (words at +0 and +4), v854 = the value of the retail dword 0x119854,
// b484 = address of the retail byte array 0x11C484 (4 entries). e.self = playerSelf (0x11BBFC), e.gl = item hash 0x10AE08.
#include "cl_env.h"
namespace {
const uint32_t kPath = 0xB2000000u | 0xD4964;   // "..\Source\D2Client\UNIT\Item.cpp"
void wr8(const cl::Env& e, uint32_t a, uint32_t v) {
    const uint32_t w = a & ~3u, s = (a & 3u) * 8;
    e.W(w, (e.R(w) & ~(0xffu << s)) | ((v & 0xffu) << s));
}
// the item hash walk: 0 = miss, ~0 = a hit whose type is not 4 (fatal 0x5A in retail)
uint32_t find(const cl::Env& e, uint32_t id) {
    for (uint32_t n = e.R(e.gl + (id & 0x7f) * 4); n; n = e.R(n + 0xe4))
        if (e.R(n + 0xc) == id) return e.R(n) == 4 ? n : ~0u;
    return 0;
}
// retail fatal: Fog error text (0xBF92 code, 0xBF80 fmt) then _exit(-1) at 0x3664; never returns
uint32_t fatal(const cl::Env& e, uint32_t code) {
    const uint32_t t = e.H(0xbf92, code);
    e.H(0xbf80, 0xB2000000u | 0xD359F, t);
    e.H(0x3664, 0xffffffffu);
    return 0;
}
uint32_t create(const cl::Env& e, uint32_t pkt, uint32_t bp) {
    const uint32_t y = e.H(0x833f0, pkt, bp, 0, 0, 0, 0, 0, 0, 0);
    if (e.blk) e.blk(e.ctx, 0x833f0, bp, 6);
    return y;
}
// the shared "item leaves the owner" tail of the hash-hit branches
void release(const cl::Env& e, uint32_t u, uint32_t it) {
    if (e.R(u) == 0) e.H(0xca0c, e.R(u + 0x60));
    if (e.H(0xc1a8, it, 0x100, 0x96, kPath) == 0) e.H(0xc544, u, it);
}
}  // namespace
extern "C" uint32_t d2c_84f00(uint32_t pkt, uint32_t g58, uint32_t v854, uint32_t b484, const cl::Env& e) {
    uint32_t B[8] = {};
    const uint32_t bp = (uint32_t)(uintptr_t)B;
    e.H(0xca90, pkt + 0xd, e.R8(pkt + 2) - 0xd, 0, bp);
    const uint32_t P = e.self;
    const uint32_t id = e.R(pkt + 9), bk = e.R8(pkt + 8);
    const uint32_t u = e.H(0xa4e20, bk, 0, 0, 0, 0, id & 0x7f, id, 0xB2000000u | (0x10a608 + bk * 0x200));
    if (!u) return 0;
    const uint32_t inv = e.R(u + 0x60);
    if (!inv || e.R(u) != 0) return 0;
    const uint32_t act = e.R8(bp + 8), id2 = e.R(pkt + 4);
    uint32_t Y = 0;
    if (u == P && act == 0) {
        const uint32_t n = find(e, id2);
        if (n == ~0u) return fatal(e, 0x5a);
        if (n) {
            if (e.H(0xc6d0, inv, n) != n) return fatal(e, 0xbf5);
            release(e, u, n);
            e.H(0xa5b00, 0, 0, 0, 0, 0, 0, e.R(n), e.R(n + 0xc));
        }
        e.H(0x8bbf0);
        const uint32_t r = e.H(0xc8a4, u, e.R(bp + 0x10), v854);
        const uint32_t w[8] = { kPath, 0xbfc, inv, e.R(bp + 4) & 0xffff, e.R(bp + 6) & 0xffff, r, 1, e.R(bp + 0x10) };
        e.H(0xc9fa, w[0], w[1], w[2], w[3], w[4]);
        if (e.blk) e.blk(e.ctx, 0xc9fa, (uint32_t)(uintptr_t)w, 8);
        if (e.R8(bp + 0x10) == 0xff) return 0;
        Y = create(e, pkt, bp);
        e.H(0x82660, e.R(bp + 6), e.R(bp + 0x10), u, 0, 0, e.R(bp + 4), Y);
        if (e.H(0xca30, Y, u)) { e.H(0x81ff0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Y); e.Ej(0x81ff0, u); }
    } else if (act == 1) {
        const uint32_t sl = e.R8(bp + 0x11);
        uint32_t X = e.H(0xc262, inv, sl);
        e.H(0x693c0, 0, 0, 0, 0, 0, 0, 0, u);
        if (!X) return 0;
        e.H(0xca2a, inv, X);
        e.H(0x69a90, 0, 0, 0, 0, 0, e.R8(pkt + 3), u);
        e.H(0xc9ca, u, inv, (uint32_t)(uintptr_t)&X, sl);
        const uint32_t it = e.H(0xc6d0, inv, X);
        if (it != X) return fatal(e, 0xc1e);
        release(e, u, it);
        e.H(0xca4e, u, it, 1, 1);
        e.H(0xcaa8, inv, sl);
        e.H(0x6a220, 0, 0, 0, 0, 0, u);
        if (X) e.H(0xa5b00, 0, 0, 0, 0, 0, 0, e.R(X), e.R(X + 0xc));
        else e.H(0xa5b00, 0, 0, 0, 0, 0, 0, 6, 0xffffffffu);
        if (u == P) {
            e.H(0x8bbf0);
            if (sl == 4) e.W(g58, e.R(g58) & 0xffff0000u);
            else if (sl == 5) e.W(g58 + 4, e.R(g58 + 4) & 0xffff0000u);
        }
        Y = create(e, pkt, bp);
        e.H(0xc9dc, Y, e.R(bp + 0x11));
        if (!e.H(0xca6c, inv, Y, (sl == 0xb || sl == 0xc) ? 4 : 3)) return 0;
        const uint32_t ok = e.H(0x81b40, 0, 0, 0, 0, 0, 0, 0, 0, Y, sl);
        e.Ej(0x81b40, inv);
        if (!ok) return 0;
        e.H(0xca18, inv, Y);
        e.H(0x68cb0, 0, 0, 0, 0, 0, u);
        const uint32_t b3 = e.R8(pkt + 3);
        if (b3 == 0 || b3 == 1) e.H(0x69a90, 0, 0, 0, 0, 0, b3, u);
        e.H(0x6a220, 0, 0, 0, 0, 0, u);
        if (u == P) {
            e.H(0x81b70, 0, 0, 0, 0, 0, Y);
            if (!e.H(0xca3c, e.R(bp + 0xc), 8)) {
                e.H(0xc7d8, inv, 0);
                e.H(0x162d0, 0, 0, 0, 0, 0, e.H(0xc202, inv));
            }
        }
        e.H(0x81ff0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Y); e.Ej(0x81ff0, u);
        e.H(0x82fe0, u);
        e.H(0x82fe0, u);
    } else {
        if (u != P) return 0;
        if (act == 2) {
            const uint32_t n = find(e, id2);
            if (n == ~0u) return fatal(e, 0x5a);
            if (n) {
                if (e.H(0xc6d0, inv, n) != n) return fatal(e, 0xc5e);
                e.H(0xcaa2, inv, e.H(0x1630, 0, 0, 0, 0, 0, 0, n));
                const uint32_t q = e.H(0x1630, 0, 0, 0, 0, 0, 0, n);
                if (q < 4) wr8(e, b484 + q, 0);
                release(e, u, n);
                e.H(0xa5b00, 0, 0, 0, 0, 0, 0, e.R(n), e.R(n + 0xc));
            }
            e.H(0x26410);
            Y = create(e, pkt, bp);
            if (e.R8(bp + 8) != 2) return fatal(e, 0xc69);
            e.H(0xca06, Y, e.R(bp + 4) & 0xffff);
            e.H(0xca48, Y, e.R(bp + 6) & 0xffff);
            if (!e.H(0xca6c, inv, Y, 2)) return fatal(e, 0xc89);
            if (!e.H(0xca84, inv, Y, e.H(0x1630, 0, 0, 0, 0, 0, 0, Y))) e.H(0x81c90);
            if (e.H(0xca30, Y, u)) { e.H(0x81ff0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Y); e.Ej(0x81ff0, u); }
            const uint32_t q = e.H(0x1630, 0, 0, 0, 0, 0, 0, Y);
            if (q < 4) wr8(e, b484 + q, 1);
            e.H(0xbe2b0);
            if (e.H(0xca3c, e.R(bp + 0xc), 0x400)) {
                e.Xs(0, 0);
                e.H(0x6b300, Y, 2, 0, 0, 0, 0x46);
            }
            if (Y) e.H(0x81aa0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Y);
            return Y;
        }
        if (act != 4) return 0;
        e.H(0x26410);
        const uint32_t n = find(e, id2);
        if (n == ~0u) return fatal(e, 0x5a);
        if (n) e.H(0xa5b00, 0, 0, 0, 0, 0, 0, 4, e.R(n + 0xc));
        Y = create(e, pkt, bp);
        e.H(0xca06, Y, e.R(bp + 4) & 0xffff);
        e.H(0xca48, Y, e.R(bp + 6) & 0xffff);
        e.H(0xc7d8, inv, Y);
        e.H(0x162d0, 0, 0, 0, 0, 0, e.H(0xc202, inv));
    }
    e.H(0xbe2b0);
    if (Y) e.H(0x81aa0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Y);
    return Y;
}
