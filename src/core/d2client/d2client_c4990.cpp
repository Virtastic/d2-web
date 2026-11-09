// 1.13c D2Client+0xC4990: fastcall ECX = unit, one unused stack word (ret 4).
// Copies the target's position (0x1630 / 0x1660) and its +4 word into the handle Ordinal 10511 returns for the unit.
#include "d2client_skillfx.h"
namespace d2c_skillfx {
u32 c4990(const Env& e, P a) {
    const u32 h = e.c310(a);
    if (!h) return 0;
    e.a0b0(a);
    const u32 t = e.rd32(a + 0x2c);
    if (!t) { e.fatal(0x946); return 0; }
    const P u = e.c2da(t);
    if (!u) return 0;
    if (!e.c6b2(u)) return 0;
    e.cb50(h, e.f1630(u));
    e.cb7a(h, e.f1660(u));
    e.cb8c(h, e.rd32(u + 4));
    return 1;
}
} // namespace d2c_skillfx
