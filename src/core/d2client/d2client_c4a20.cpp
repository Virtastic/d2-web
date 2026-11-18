// 1.13c D2Client+0xC4A20: fastcall ECX = unit, one unused stack word (ret 4).
// As 0xC4990 without the Ordinal 10096 gate: the target's path x / y (types 2, 4, 5 read the path words, the others ask C3BE / C2EC).
#include "d2client_skillfx.h"
namespace d2c_skillfx {
static bool pathTyped(u32 t) { return t == 2 || (t > 3 && t <= 5); }
u32 c4a20(const Env& e, P a) {
    const u32 h = e.c310(a);
    if (!h) return 0;
    e.a0b0(a);
    const u32 t = e.rd32(a + 0x2c);
    if (!t) { e.fatal(0x946); return 0; }
    const P u = e.c2da(t);
    if (!u) return 0;
    P pb = e.rd32(u + 0x2c);
    e.cb50(h, pathTyped(e.rd32(u)) ? e.rd32(pb + 0xc) : (pb ? e.c3be(pb) : 0));
    pb = e.rd32(u + 0x2c);
    e.cb7a(h, pathTyped(e.rd32(u)) ? e.rd32(pb + 0x10) : (pb ? e.c2ec(pb) : 0));
    return 1;
}
} // namespace d2c_skillfx
