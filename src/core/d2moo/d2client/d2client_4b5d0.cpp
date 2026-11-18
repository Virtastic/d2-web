// 1.13c D2Client+0x4B5D0 -- the town-portal / waypoint transition state handler, written from the disassembly (tail of 0x4BC80).
// No arguments; g = a pinned globals block, one slot per module global (RVA in brackets):
//  +0x00 phase byte [0x119781]   +0x04 [0x11975F]   +0x08 [0x119663]   +0x0C [0x11BC34]   +0x10 [0x11975B]   +0x14 [0x119729]
//  +0x18 [0x119737]   +0x1C [0x11B834]   +0x20 [0x11B828]   +0x24 [0x11B824]   +0x28 [0x119753]   +0x2C [0x11978E]   +0x30 [0x119786]
//  +0x34 [0x11C2F8]   +0x38 [0x11C2F4]   +0x3C 9 bytes [0x1222F8..0x122300]   +0x48 playerSelf [0x11BBFC]   +0x50 the 128-slot unit hash [0x10AE08]
// Phase 0..6: 0 hands EBX=old [0x119753] to 0x88A70(0,0,0,0), clears it and looks the unit [0x119786] up (hash chain +0xE4, id +0xC, type +0):
// none: [0x11978E]=1, return; type 4: tail; other type: fatal (never vectored). 1 does the same with EBX=0xDD (none: tail, type 4: flag, return).
// 5 calls 0x5AC00 first. Tail: [0x11978E]=0, release [0x11975F] through 0xA3D20, clear the 0x11C2F8/F4 pair and the 9 bytes at 0x1222F8,
// [0x119663]=3, 0x8F660 then 0x94970. Phase >= 7: release [0x11975F]; [0x119663] must be 5 (else fatal), set to 6, [0x11BC34]=7, [0x11975B] must be 0
// (else fatal); with [0x119729]==2 the ownership test 0xC202([player+0x60]) may call 0x162D0; the 11-word 0xA3E40 builds the window into [0x11975B],
// flag +0x5C=1 and two 0xA36F0 lines whose captions come from 0xD1EC (string id by phase, then 0xD4B).
#include "cl_env.h"
extern "C" void d2c_4b5d0(const cl::Env& e, uint32_t g) {
    auto fatal = [&](uint32_t id) { uint32_t m = e.H(0xbf92, id); e.H(0xbf80, 0xd359f, m); e.H(0x3664, 0xffffffffu); };
    uint32_t pl = e.R(g + 0x48), ebx = e.R(pl + 0x60), st = e.R8(g);
    if (st > 6) {
        uint32_t o = e.R(g + 4);
        if (o) { e.H(0xa3d20); e.W(g + 4, 0); }   // ECX holds [0x11975F] at the call, but 0xA3D20 takes no register argument
        uint32_t sid = 0xd21;
        switch (st) {
            case 7: case 8: sid = 0xd1e; break;
            case 10: sid = 0xd1c; break;
            case 11: sid = 0xd20; break;
            case 12: sid = 0xd22; break;
            case 15: sid = 0xd23; break;
            case 16: sid = 0xd1d; break;
            default: break;
        }
        if (e.R(g + 8) != 5) { fatal(0x13ad); return; }
        e.W(g + 8, 6);
        e.W(g + 0xc, 7);
        if (e.R(g + 0x10)) { fatal(0x1399); return; }
        if (e.R(g + 0x14) == 2) {
            uint32_t r = e.H(0xc202, ebx);
            if (e.R(g + 0x18) == 1 && r && e.R(g + 0x1c) == 0) e.H(0x162d0, 0, 0, 0, 0, 0, r);
        }
        e.Xs(1, 1);
        uint32_t w = e.H(0xa3e40, e.R(g + 0x20), e.R(g + 0x24), 0x6fafaaf0, 0, 0);   // the item callback 0x4AAF0 at its image address
        e.W(g + 0x10, w);
        if (!w) { fatal(0x529); return; }
        e.W(w + 0x5c, 1);
        uint32_t s1 = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, sid);
        e.Xs(0);
        e.H(0xa36f0, s1, 0x15, 0, 1, 0, 1, w);
        uint32_t s2 = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xd4b);
        e.Xs(1);
        e.H(0xa36f0, s2, 0xf, 0, 1, 0x6fafaaf0, 0, w);
        return;
    }
    uint32_t id = e.R(g + 0x30);
    uint32_t u = e.R(g + 0x50 + 4 * (id & 0x7f));
    if (st == 0) {
        uint32_t old = e.R(g + 0x28);
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, old);
        e.W(g + 0x28, 0);
        u = e.R(g + 0x50 + 4 * (id & 0x7f));
        while (u && e.R(u + 0xc) != id) u = e.R(u + 0xe4);
        if (!u) { e.W(g + 0x2c, 1); return; }
        if (e.R(u) != 4) { fatal(0x5a); return; }
    } else if (st == 1) {
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 0xdd);
        u = e.R(g + 0x50 + 4 * (id & 0x7f));
        while (u && e.R(u + 0xc) != id) u = e.R(u + 0xe4);
        if (u) {
            if (e.R(u) != 4) { fatal(0x5a); return; }
            e.W(g + 0x2c, 1);
            return;
        }
    } else if (st == 5) {
        e.H(0x5ac00);
    }
    e.W(g + 0x2c, 0);
    if (e.R(g + 4)) { e.H(0xa3d20); e.W(g + 4, 0); }
    e.W(g + 0x34, 0);
    e.W(g + 0x38, 0);
    e.W(g + 0x3c, e.R(g + 0x3c) & ~0xffu);
    e.W(g + 0x3d, 0);
    e.W(g + 0x41, 0);
    e.W(g + 8, 3);
    e.H(0x8f660);
    e.H(0x94970);
}
