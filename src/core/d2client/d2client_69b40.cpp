// 1.13c D2Client+0x69B40 addLink, written from the disassembly.
// EAX = unit, stack p8 (a), p12 (id), ret 8. A 0x104-byte name buffer and a dword local live on the stack.
// Unit kind 0..2 only. The +0x60 word comes from the unit, or from 0xA60D0(unit) when the unit's flag (+0xC8 bit 3) is set and
// D2Common #10650(unit, 0x25) says so. D2Common 0xCC1C(unit, &name, &local, 1, 1, word, id) fills the name and the local; an empty name
// ends it (0). pData = unit[+0x54] (null ends it). 0x69870(EAX=id, ECX=unit, EBX=&out; local, a) nonzero: a 0x14-byte node is
// allocated (0xBF7A, fatal chain on null), zeroed, and the table is 0x114CD8 or, when 0x1A80() is nonzero, 0x1144C8; the name is looked up in it
// by 0x69350(EDX=&name; table) then 0x69970(EAX=&name; table); none: 0. Found: node{id, local, found, pData[0x28]}; pData[0x28] = node.
// Whichever way, the result is 0x69090(EAX=unit, ECX=id).
#include <cstring>
#include "cl_env.h"
extern "C" uint32_t d2c_69b40(uint32_t unit, uint32_t a, uint32_t id, const cl::Env& e) {
    uint32_t nb[65];
    memset(nb, 0, sizeof nb);
    uint32_t loc = 0, loc2 = 0;
    if (!unit) return 0;
    const int32_t kind = (int32_t)e.R(unit);
    if (kind < 0 || kind > 2) return 0;
    uint32_t w;
    uint32_t src = unit;
    if ((e.R(unit + 0xc8) >> 3) & 1) {
        if (e.H(0xcbda, unit, 0x25)) {
            const uint32_t r = e.H(0xa60d0, 0, 0, 0, 0, 0, unit);
            if (r) src = r;
        }
    }
    w = e.R(src + 0x60);
    const uint32_t np = (uint32_t)(uintptr_t)nb, lp = (uint32_t)(uintptr_t)&loc;
    e.Xs(w, id);
    e.H(0xcc1c, unit, np, lp, 1, 1);
    if (!(nb[0] & 0xff)) return 0;
    const uint32_t pd = e.R(unit + 0x54);
    if (!pd) return 0;
    if (e.H(0x69870, loc, a, 0, 0, 0, id, unit, 0, (uint32_t)(uintptr_t)&loc2)) {
        const uint32_t node = e.H(0xbf7a, 0x434, 0, 0, 0, 0, 0, 0x14, 0xB20D4FA4u);
        if (!node) {
            const uint32_t m = e.H(0xbf92, 0x435);
            e.H(0xbf80, 0xB20D359Fu, m);
            e.H(0x3664, 0xffffffffu);
            return 0;
        }
        for (uint32_t o = 0; o < 0x14; o += 4) e.W(node + o, 0);
        const uint32_t tbl = e.H(0x1a80) ? 0xB21144C8u : 0xB2114CD8u;
        uint32_t f = e.H(0x69350, tbl, 0, 0, 0, 0, 0, 0, np);
        if (e.blk) e.blk(e.ctx, 0x69350, np, 16);
        if (!f) {
            f = e.H(0x69970, tbl, 0, 0, 0, 0, np);
            if (e.blk) e.blk(e.ctx, 0x69970, np, 16);
            if (!f) return 0;
        }
        e.W(node, id);
        e.W(node + 4, loc);
        e.W(node + 8, f);
        e.W(node + 0x10, e.R(pd + 0x28));
        e.W(pd + 0x28, node);
    }
    return e.H(0x69090, 0, 0, 0, 0, 0, unit, id);
}
