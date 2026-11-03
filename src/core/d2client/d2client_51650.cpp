// 1.13c D2Client+0x51650 -- nearest-target pick (EAX = skill/record, stack: unit, unused, &x, &y; ret 0x10). Returns the unit.
// The record byte +7 ANDed with the first byte of the gdwBitMasks array (D2Client slot 0xCEE50) selects the "filtered" mode: a unit with flag
// 0x10000 in +0xC4, a type-0 unit in mode 0 or 0x11, or a type-1 unit in mode 0 or 0xC is returned as it stands. A non-null unit writes its
// position (D2Common 0xC3BE / 0xC2EC, or the path words +0xC/+0x10 for unit types 2 and 4..5) through the two out pointers. The scan count is 0x2003
// (+0xFFF when filtered); D2Common 0xC42A(unit, file string 0x6FB85F24, 0x301) == 0x5A narrows it to 0x20 with callback 0x511D0 (a type-4 unit
// answering 0xCCEE(unit+4) is returned first). Request block (14 dwords): [0] count, [2] player slot value, [3] x, [4] y, [5] 9. 0xCCFA(0, &S,
// 0xC1F0(player) or 0, x, y, 9, callback, &req) fills the scan struct S, S[5] = count, 0xCCD0(&S) runs it; S[1] is the unit array and S[6] its length. Each
// entry whose 0xA68E0(unit, 0, 0, 0) is non-zero and whose 0xC4D8(unit, x, y) is below the best so far (start 10) becomes the result; 0xCCE8(&S) closes.
#include "cl_env.h"
extern "C" uint32_t d2c_51650r(uint32_t rec, uint32_t u, uint32_t px, uint32_t py, const cl::Env& e) {
    uint32_t m = e.R8(e.bm) & e.R8(rec + 7);
    if (u) {
        if (m) {
            if (e.R(u + 0xc4) & 0x10000) return u;
            uint32_t t = e.R(u), mode = e.R(u + 0x10);
            if (t == 0 && (mode == 0 || mode == 0x11)) return u;
            if (t == 1 && (mode == 0 || mode == 0xc)) return u;
        }
        e.W(px, e.PD(u, 0xc3be, 0xc));
        e.W(py, e.PD(u, 0xc2ec, 0x10));
    }
    uint32_t cnt = (m ? 0xfffu : 0u) + 0x2003, cb = 0;
    if (e.H(0xc42a, u, 0xB20D5F24u, 0x301) == 0x5a) {
        if (u && e.R(u) == 4 && e.H(0xccee, e.R(u + 4))) return u;
        cnt = 0x20; cb = 0x6fb011d0u;
    }
    uint32_t req[14] = {};
    req[0] = cnt; req[2] = e.self; req[3] = e.R(px); req[4] = e.R(py); req[5] = 9;
    (void)cb;
    uint32_t uv = e.self ? e.H(0xc1f0, e.self) : 0;
    uint32_t S[8] = {};
    uint32_t sp = (uint32_t)(uintptr_t)S;
    e.H(0xccfa, 0, sp, uv, e.R(px), e.R(py));
    if (e.blk) e.blk(e.ctx, 0xccfa, (uint32_t)(uintptr_t)req, 14);
    S[5] = cnt;
    e.H(0xccd0, sp);
    if (e.blk) e.blk(e.ctx, 0xccd0, sp, 7);
    int32_t best = 10;
    if ((int32_t)S[6] > 0) {
        for (int32_t i = 0;;) {
            uint32_t v = e.R(S[1] + (uint32_t)i * 4);
            int32_t d = (int32_t)e.H(0xc4d8, v, e.R(px), e.R(py));
            uint32_t ok = e.H(0xa68e0, v, 0, 0, 0);
            if (ok && d < best) { u = v; best = d; }
            if (!(++i < (int32_t)S[6])) break;
        }
    }
    e.H(0xcce8, sp);
    return u;
}
