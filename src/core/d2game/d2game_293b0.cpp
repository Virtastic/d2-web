// 1.13c D2Game+0x293B0 -- dispatch a skill request record (ECX = game, EDX = unit, [esp+4] = a, [esp+8] = b, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (see d2game_c310.h).
// rec = 0xA3F4(unit, a); none -> 0. Flags = 0xA79C(rec). Bit 8: 0x28740(unit; game, rec) zero -> 1; 0x272C0(unit; game, rec) set -> 0xE2310(game, 0x10), 1;
// else walk the unit's +0xDC node chain (+0x24 next, byte 0 == 1: 0x8E6F0(node)), then 0x8EA80(game, unit; 1, game +0xA8 + 4, 0, 0), 0.
// Bit 7 (sign of the low byte): return 0x26A80(unit; rec). Bit 9: return 0x27820(unit; game, a, b). Else 0xA78A(rec, 0x1000), 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f293b0(GE& e, uint32_t game, uint32_t unit, uint32_t a, uint32_t b)
{
    const uint32_t rec = e.h(0xA3F4, 0, unit, a);
    if (!rec) return 0;
    const uint32_t fl = e.h(0xA79C, 0, 0, 0, 0, 0, rec);
    if (fl & 0x100) {
        if (!e.h(0x28740, unit, 0, 0, 0, 0, game, rec)) return 1;
        if (e.h(0x272C0, 0, 0, 0, 0, 0, game, rec)) { e.h(0xE2310, 0, 0, 0, 0, 0, game, 0x10); return 1; }
        if (unit) {
            for (uint32_t n = e.rd32(unit + 0xDC); n;) {
                const uint32_t next = e.rd32(n + 0x24);
                if ((e.rd32(n) & 0xFF) == 1) e.h(0x8E6F0, n);
                n = next;
            }
        }
        e.h(0x8EA80, 0, game, unit, 0, 0, 1, e.rd32(game + 0xA8) + 4, 0, 0);
        return 0;
    }
    if ((int8_t)fl < 0) return e.h(0x26A80, 0, 0, 0, 0, 0, rec);
    if (e.h(0xA79C, 0, 0, 0, 0, 0, rec) & 0x200) return e.h(0x27820, unit, 0, 0, 0, 0, game, a, b);
    e.h(0xA78A, 0, 0, 0, 0, 0, rec, 0x1000);
    return 1;
}
}
