// 1.13c D2Game+0x294A0 -- queue a skill's cast (ECX = game, EDX = unit, [esp+4] = skill id, [esp+8] = level, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (see d2game_c310.h).
// Skill record out of range -> 0. rec = 0xA3F4(unit, skill); none -> 0. Walk the unit's +0xDC node chain (byte 0 == 1: 0x8E6F0(node), EDI = game).
// Flags = 0xA79C(rec). Bit 8: 0x28740(unit; game, rec) zero, or the unit a type-1 unit -> 1; else 0xAC9A(unit, record +0x138, skill, level) and,
// for a unit, queue a zeroed 28-word request (word 1 = 9) through 0xA0C70 (frames 0x8583), 1. Bit 7: return 0x26A80(unit; rec). Else a non-type-1 unit gets 0xA78A(rec, 0), 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f294a0(GE& e, uint32_t T, uint32_t game, uint32_t unit, uint32_t sk, uint32_t lv)
{
    const uint32_t srec = e.skillRec(T, (int32_t)sk);
    if (!srec) return 0;
    const uint32_t rec = e.h(0xA3F4, 0, unit, sk);
    if (!rec) return 0;
    uint32_t isU = 0;
    if (unit) for (uint32_t n = e.rd32(unit + 0xDC); n;) {
        const uint32_t next = e.rd32(n + 0x24);
        if ((e.rd32(n) & 0xFF) == 1) e.h(0x8E6F0, n);
        n = next;
    }
    isU = unit ? (e.rd32(unit) == 1) : 0;
    const uint32_t fl = e.h(0xA79C, 0, 0, 0, 0, 0, rec);
    if (fl & 0x100) {
        if (!e.h(0x28740, unit, 0, 0, 0, 0, game, rec) || isU) return 1;
        uint32_t blk[28] = {}; blk[1] = 9;
        const uint32_t ac = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(srec + 0x138), sk, lv);
        if (unit) e.hs(0xA0C70, game, 0, 0, 0, 0, { unit, isU, isU, ac, 0x8583, 0, e.stk(blk), isU }, 6, 28);
        return 1;
    }
    if ((int8_t)fl < 0) return e.h(0x26A80, 0, 0, 0, 0, 0, rec);
    if (!isU) e.h(0xA78A, 0, 0, 0, 0, 0, rec, 0);
    return 1;
}
}
