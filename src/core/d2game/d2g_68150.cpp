// 1.13c D2Game+0x68150 -- ECX = game, EDX = unit, one unused stack word (ret 4). Written from the 1.13c disassembly.
// Only a unit of class 0x92 counts (else 0). Its player data (+0x14, type 1) list at +0x30 is walked through +8: the first
// node with a nonzero head hands the unit to 0x48410(game, unit, 0x62A30) and returns 1. Otherwise 0xA496(unit, 0), an
// 8-dword request {0, unit, 0.., byte +0x15 = 0x64} to 0x75E10(game, &req, 1), and the type-4 quest's data+0x61 is set. Returns 1.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f68150(HookEnv& e, uint32_t game, uint32_t unit)
{
    if (!unit || e.rd32(unit + 4) != 0x92) return 0;
    uint32_t lst = 0;
    if (e.rd32(unit) == 1 && e.rd32(unit + 0x14)) lst = e.rd32(e.rd32(unit + 0x14) + 0x30);
    for (uint32_t n = e.rd32(lst); n; n = e.rd32(n + 8))
        if (e.rd32(n)) { e.h(0x48410, 0, 0, 0, 0, 0, game, unit, 0xB2000000u | 0x62A30); return 1; }
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    uint32_t req[8] = { 0, unit, 0, 0, 0, 0x6400, 0, 0 };
    e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(req), 1);
    e.blk(0x75E10, req, 8);
    const uint32_t qs = e.rd32(game + 0x10F4);
    if (!qs) return 1;
    for (uint32_t n = e.rd32(qs); n; n = e.rd32(n + 0xF4))
        if (e.rd32(n) == 4) { e.wr8(e.rd32(n + 0x18) + 0x61, 1); break; }
    return 1;
}
}
