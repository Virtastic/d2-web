// 1.13c D2Client+0x6AC60, written from the disassembly. EAX = unit, stdcall 2 (p8 = mode, p12), returns 1, 0 or -1.
// Flag word: a unit with bit 3 of +0xC8 set and 0xCBDA(unit, 0x25) nonzero reads +0x60 of 0xA60D0(EAX=unit) (unit +0x60 when that is null);
// 0xC2AA(unit, w60, &L24, p8, 0) gives the weapon class. 0x69910(EAX=unit, ECX=p8) and 0x6A2F0(EAX=p8, ECX=wclass, EDX=unit) both null:
// sprintf "sAsyncLoadBlock failed ..." into a stack buffer, return 0. Else the node of id p8 on [[unit+0x54]+0x28] (next +0x10) gives
// tbl = [[node+8]+0x14]; a missing link, node or tbl returns 0. b = 0xC610(unit) & 0xFF; 0xC65E(unit) presets x; 0xC658(unit, &type,
// &class, &x, "Gfx.cpp", 0x6CD). b < 0x40: 0x6D960(EAX=type, EBX=p8, EDI=class; unit) then idx = 0xC112(b, tbl[2]); else idx = 0.
// row = tbl + 0x1C + 9*tbl[0] + (tbl[0]*idx + 1)*tbl[1]; for each of tbl[0] bytes c (c >= 0x10: fatal 0x6DD) a zeroed 0x48-byte frame
// {+5 = c, +0x40 = 0xC610(unit) & 0xFF} goes to 0x6E2C0(EAX=p8; unit, &frame, &L2C); success then 0xC0D0(&frame, p12, p12) == 0 runs
// 0xC118(&frame) (after +0x34 = 0): zero returns -1, frame byte +0x44 bit 2 clear clears the result.
#include <cstring>
#include "cl_env.h"
extern "C" uint32_t d2c_6ac60(uint32_t unit, uint32_t p8, uint32_t p12, const cl::Env& e) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    uint32_t res = 1, w60;
    if ((e.R(unit + 0xc8) >> 3 & 1) && e.H(0xcbda, unit, 0x25)) {
        const uint32_t q = e.H(0xa60d0, 0, 0, 0, 0, 0, unit);
        w60 = q ? e.R(q + 0x60) : e.R(unit + 0x60);
    } else {
        w60 = e.R(unit + 0x60);
    }
    uint32_t l24 = 0;
    const uint32_t wc = e.H(0xc2aa, unit, w60, P(&l24), p8, 0);
    if (!e.H(0x69910, 0, 0, 0, 0, 0, unit, p8) && !e.H(0x6a2f0, 0, 0, 0, 0, 0, p8, wc, unit)) {
        char buf[280];
        e.H(0x3146, P(buf), 0xB20D4F40u, p12, e.R(unit), e.R(unit + 4));
        if (e.blk) { uint32_t a[8] = { P(buf), 0xB20D4F40u, p12, e.R(unit), e.R(unit + 4), p8, l24, wc }; e.blk(e.ctx, 0x3146, P(a), 8); }
        return 0;
    }
    const uint32_t pa = e.R(unit + 0x54);
    if (!pa) return 0;
    uint32_t n = e.R(pa + 0x28);
    if (!n) return 0;
    while (e.R(n) != p8) { n = e.R(n + 0x10); if (!n) return 0; }
    const uint32_t tbl = e.R(e.R(n + 8) + 0x14);
    if (!tbl) return 0;
    const uint32_t b2 = e.R8(tbl + 2);
    const uint32_t b = e.H(0xc610, unit) & 0xff;
    uint32_t type = e.R(unit), cls = e.R(unit + 4), x = e.H(0xc65e, unit);
    e.Xs(0x6cd);
    e.H(0xc658, unit, P(&type), P(&cls), P(&x), 0xB20D4FA4u);
    uint32_t idx = 0;
    if (b < 0x40) {
        e.H(0x6d960, unit, 0, 0, 0, 0, type, 0, 0, p8);
        e.Edj(0x6d960, cls);
        idx = e.H(0xc112, b, b2);
    }
    const uint32_t n0 = e.R8(tbl), row = tbl + n0 * 9 + (n0 * idx + 1) * e.R8(tbl + 1) + 0x1c;
    if (!row) { e.H(0xbf80, 0xB20D359Fu, e.H(0xbf92, 0x6d6)); e.H(0x3664, 0xffffffffu); return 0xffffffffu; }
    if ((int32_t)n0 <= 0) return res;
    for (uint32_t i = 0; (int32_t)i < (int32_t)e.R8(tbl); ++i) {
        const uint32_t c = e.R8(row + i);
        if (c >= 0x10) { e.H(0xbf80, 0xB20D359Fu, e.H(0xbf92, 0x6dd)); e.H(0x3664, 0xffffffffu); return 0xffffffffu; }
        uint32_t fr[18] = {}, snap[18], l2c = 0;
        fr[0x40 / 4] = e.H(0xc610, unit) & 0xff;
        ((uint8_t*)fr)[5] = (uint8_t)c;
        memcpy(snap, fr, sizeof fr);
        const uint32_t ok = e.H(0x6e2c0, unit, P(fr), P(&l2c), 0, 0, p8);
        if (e.blk) e.blk(e.ctx, 0x6e2c0, P(snap), 18);
        if (!ok) continue;
        memcpy(snap, fr, sizeof fr);
        const uint32_t r0 = e.H(0xc0d0, P(fr), p12, p12);
        if (e.blk) e.blk(e.ctx, 0xc0d0, P(snap), 18);
        if (r0) continue;
        fr[0x34 / 4] = 0;
        memcpy(snap, fr, sizeof fr);
        const uint32_t r1 = e.H(0xc118, P(fr));
        if (e.blk) e.blk(e.ctx, 0xc118, P(snap), 18);
        if (!r1) return 0xffffffffu;
        if (!(((const uint8_t*)fr)[0x44] & 4)) res = 0;
    }
    return res;
}
