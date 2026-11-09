// 1.13c D2Client+0xC45C0: fastcall ECX = unit, EDX = p2, stack s0 = skill level (ret 4).
// Asks Ordinal C1F0 for the units around the caster (query 0x3002, radius from skill row 0x11D's level fields), then for each
// one creates missile 0x24B through 0xB30F0 and delays it by its 10534 frame count plus a seeded (unit +0x20 / +0x24) -10..+9.
#include "d2client_skillfx.h"
namespace d2c_skillfx {
static bool pathTyped(u32 t) { return t == 2 || (t > 3 && t <= 5); }
static u32 posX(const Env& e, P o) { P pb = e.rd32(o + 0x2c); return pathTyped(e.rd32(o)) ? e.rd32(pb + 0xc) : (pb ? e.c3be(pb) : 0); }
static u32 posY(const Env& e, P o) { P pb = e.rd32(o + 0x2c); return pathTyped(e.rd32(o)) ? e.rd32(pb + 0x10) : (pb ? e.c2ec(pb) : 0); }
u32 c45c0(const Env& e, P u, u32 p2, i32 p3) {
    u32 cr = 0;
    if ((i32)e.rd32(e.dt + 0xba0) > 0x11d) {
        const u32 t = e.rd32(e.dt + 0xb98) + 0x27ccc;
        if (t && p3 > 0) cr = (u32)(p3 - 1) * e.rd32(t + 0x15c) + e.rd32(t + 0x158);
    }
    const u32 w = posX(e, u), h = posY(e, u);
    u32 q[14] = {};
    q[0] = 0x3002; q[2] = (u32)u; q[3] = w; q[4] = h; q[5] = cr;
    const u32 r = e.c1f0(u, w, h, cr, 0, q);
    u32 E[8] = {};                 // the 0x1C..0x3B stack record CCFA / CCD0 / CCE8 share: [1] = list, [5] = CCFA's slot, [6] = count
    e.ccfa(0, &E[5], r);
    E[5] = q[0];
    e.ccd0(E);
    for (i32 i = 0; i < (i32)E[6]; ++i) {
        const P o = e.rd32(E[1] + 4u * (u32)i);
        const u32 oy = posY(e, o), ox = posX(e, o);
        u32 s[23] = {};
        s[0] = 1; s[1] = (u32)u; s[4] = 0x24b; s[5] = ox; s[6] = oy; s[11] = p2; s[12] = (u32)p3;
        const P m = e.fb30f0(s);
        if (!m) continue;
        const u32 b = (u32)e.c430(m);
        const uint64_t z = (uint64_t)e.rd32(u + 0x20) * 0x6ac690c5u + e.rd32(u + 0x24);
        e.wr32(u + 0x24, (u32)(z >> 32));
        e.wr32(u + 0x20, (u32)z);
        const i32 v = (i32)(b + (u32)z % 20u - 10u);
        e.c4a2(m, v);
        e.c2c8(m, v);
    }
    e.cce8(E);
    return 1;
}
} // namespace d2c_skillfx
