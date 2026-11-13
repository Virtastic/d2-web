// 1.13c D2Game+0xDB4C0 -- unit damage tail (ECX = unit, EDX = value, one unused stack word); written from the 1.13c disassembly.
// A "plain" unit is one that is set, has bit 0 of byte +0xC6 clear and, by type (+0), has +0x10 nonzero and not 0x11 (type 0) or not 0xC (type 1); other types never are.
// A unit that is not plain first runs 0xA7E4(unit, value); a nonzero result is the return. Then 0xA3CA(unit, value, 0); a plain unit also runs 0xA3CA(unit, 0x6B, 0).
// The result is 0xA59E(unit, source file SUnitDmg.cpp, line 0x14E).
#include "../fn/hookenv.h"
namespace d2g {
static bool plain_db4c0(HookEnv& e, uint32_t u)
{
    if (!u || (e.rd8(u + 0xC6) & 1)) return false;
    const uint32_t t = e.rd32(u), k = e.rd32(u + 0x10);
    if (t == 0) return k != 0 && k != 0x11;
    if (t == 1) return k != 0 && k != 0xC;
    return false;
}
uint32_t fdb4c0(HookEnv& e, uint32_t u, uint32_t v)
{
    if (!plain_db4c0(e, u)) {
        const uint32_t r = e.h(0xA7E4, 0, 0, 0, 0, 0, u, v);
        if (r) return r;
    }
    e.h(0xA3CA, 0, 0, 0, 0, 0, u, v, 0);
    if (plain_db4c0(e, u)) e.h(0xA3CA, 0, 0, 0, 0, 0, u, 0x6B, 0);
    return e.h(0xA59E, 0, 0, 0, 0, 0, u, 0xB2000000u | 0xFB0EC, 0x14E);
}
}
