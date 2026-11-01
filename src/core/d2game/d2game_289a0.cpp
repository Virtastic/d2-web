// 1.13c D2Game+0x289A0 -- apply a frenzy-style skill hit: damage set-up plus aura stat-list refresh (ECX = game, EDX = unit, [esp+4] = skill id, [esp+8] = level, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (see d2game_gp.h).
// Skill record out of range -> 0. Refresh the target (0xE07A0), resolve unit +0x2C (0xA598); none or itself -> 0. r = 0xACD6(unit, skill, level);
// fl = 0xDE5A0(game, r, 0; EAX = target, EDI = unit). A zeroed 0x70-byte request: word at +4 = low 16 bits of fl. When fl bit 0 is set the record
// feeds it: word +4 |= record word +0x12E, +0 |= record +0x130, +0x60 = record +0x134 when nonzero, +0xC = 0xAC9A(unit, record +0x138, skill, level), and with the byte
// at record +0x1DC set +0x68 = 0xAC9A(.., record +0x144, ..) and, when above zero, byte +0x65 = that byte; then 0x9F210(block, level; EAX = skill, EDI = unit).
// 0xDDDE0(game, target; EAX = unit, CL = record byte +0x1A5 or 0x80, EDI = block). The aura state (short at +0x80) outside the state table -> 1. frame = game +0xA8 +
// 0xAC9A(unit, record +0x13C, ..), kept above game +0xA8 (else +10). Stat list for the state (0xA5D4), or made by 0xA6A0 (none -> 1) with 0xA5B6, 0xA75A, 0xA60A, 0xA3CA;
// 0xA7C6(list, frame), 0x8EA80(game, unit; 0xC, frame, 0, 0, 0), 0x448E0(record; unit, list, skill, level), 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f289a0(GE& e, uint32_t T, uint32_t game, uint32_t unit, uint32_t sk, uint32_t lv)
{
    const uint32_t srec = e.skillRec(T, (int32_t)sk);
    if (!srec) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit || !tgt) return 0;
    const uint32_t r = e.h(0xACD6, 0, 0, 0, 0, 0, unit, sk, lv);
    const uint32_t fl = e.h(0xDE5A0, tgt, 0, 0, 0, 0, game, r, 0);
    uint32_t blk[28] = {};
    uint32_t w16 = fl & 0xFFFF;
    if (fl & 1) {
        w16 |= (e.rd32(srec + 0x12C) >> 16) & 0xFFFF;
        blk[0] |= e.rd32(srec + 0x130);
        if (e.rd32(srec + 0x134)) blk[0x18] = e.rd32(srec + 0x134);
        blk[3] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(srec + 0x138), sk, lv);
        const uint32_t b = e.rd32(srec + 0x1DC) & 0xFF;
        if (b) {
            const uint32_t v = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(srec + 0x144), sk, lv);
            blk[0x1A] = v;
            if ((int32_t)v > 0) blk[0x19] = (blk[0x19] & ~0xFF00u) | (b << 8);
        }
        blk[1] = w16;
        e.hs(0x9F210, sk, 0, 0, 0, 0, { e.stk(blk), lv }, 0, 28);
    }
    blk[1] = w16;
    e.h(0xDDDE0, unit, 0, 0, 0, 0, game, tgt);
    const int32_t st = (int16_t)(e.rd32(srec + 0x80) & 0xFFFF);
    if (st < 0 || st >= (int32_t)e.rd32(T + 0xC4)) return 1;
    const uint32_t base = e.rd32(game + 0xA8);
    uint32_t fr = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(srec + 0x13C), sk, lv) + base;
    if ((int32_t)fr <= (int32_t)base) fr = base + 10;
    uint32_t sl = e.h(0xA5D4, 0, 0, 0, 0, 0, unit, (uint32_t)st);
    if (!sl) {
        sl = e.h(0xA6A0, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), 2, fr, e.rd32(unit), e.rd32(unit + 0xC));
        if (!sl) return 1;
        e.h(0xA5B6, 0, 0, 0, 0, 0, sl, (uint32_t)st);
        e.h(0xA75A, 0, 0, 0, 0, 0, sl, 0xB209F7A0);
        e.h(0xA60A, 0, 0, 0, 0, 0, unit, sl, 1);
        e.h(0xA3CA, 0, 0, 0, 0, 0, unit, (uint32_t)st, 1);
    }
    e.h(0xA7C6, 0, 0, 0, 0, 0, sl, fr);
    e.h(0x8EA80, 0, game, unit, 0, 0, 0xC, fr, 0, 0, 0);
    e.h(0x448E0, srec, 0, 0, 0, 0, unit, sl, sk, lv);
    return 1;
}
}
