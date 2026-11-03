// D2Game+0x509E0 -- SkillNec_ApplyToUnit: EAX = source unit (may be 0), stack (game, target, skill id, level), ret 0x10. Skill id 0: return; ids below 0 or at/above dt +0xBA0 leave the row 0 (retail then faults on row +0x13C).
// Source (type, id) = (dword +0, dword +0xC), or (6, -1) with no source. A non-zero target of type 1 gets, in the record at [[target +0x14] +0x28]: +0x2C = id, +0x30 = type, +0x28 = game; then +0xC = source id, +0x10 = source type
// (-1 and 1 with no source). Each node of type byte 2 in the list at target +0xDC (next +0x24) goes to 0x8E6F0(EAX = node). Then 0x8EA80(ECX = game, EDX = target; 2, game word +0xA8 + 0xF, 0, 0, 0),
// 0xE0460(EAX = target, EBX = 2; 0): target non-zero: type 1 with a [target +0x14] record clears its dwords +0x34 and +0x38, and target flags +0xC4 |= 0x80000000; 0xA3CA(target, 0x60, 1);
// n = 0xAC9A(source, row +0x13C, skill, level); n > 0: 0x232B0(EBX = 0x15; target, 0) and 0x8EA80(ECX = game, EDX = target; 7, game word +0xA8 + n, 0, 0, 0).
#include "d2game_seam4e.h"
// 1.13c D2Game+0x509E0
extern "C" void d2game_509e0(uint32_t src, uint32_t game, uint32_t tgt, int32_t skill, uint32_t lvl, uint32_t dt, const D2Seam* e)
{
    if (!skill) return;
    uint32_t row = 0;
    if (skill >= 0 && skill < (int32_t)d2rd32(dt + 0xBA0)) row = (uint32_t)skill * 0x23C + d2rd32(dt + 0xB98);
    const uint32_t sid = src ? d2rd32(src + 0xC) : 0xFFFFFFFFu;
    const uint32_t sty = src ? d2rd32(src) : 6;
    auto rec = [&]() -> uint32_t { const uint32_t p = d2rd32(tgt + 0x14); return p ? d2rd32(p + 0x28) : 0; };
    if (tgt) {
        if (d2rd32(tgt) == 1) { const uint32_t r = rec(); d2wr32(r + 0x2C, sid); d2wr32(r + 0x30, sty); d2wr32(r + 0x28, game); }
        if (d2rd32(tgt) == 1) {
            const uint32_t r = rec();
            if (src) { d2wr32(r + 0xC, d2rd32(src + 0xC)); d2wr32(r + 0x10, d2rd32(src)); }
            else { d2wr32(r + 0xC, 0xFFFFFFFFu); d2wr32(r + 0x10, 1); }
        }
        for (uint32_t n = d2rd32(tgt + 0xDC); n;) {
            const uint32_t nx = d2rd32(n + 0x24);
            if (d2rd8(n) == 2) e->call(0x8E6F0, n);
            n = nx;
        }
    }
    e->call(0x8EA80, 0, game, tgt, 0, 0, 2, d2rd32(game + 0xA8) + 0xF, 0, 0, 0);
    e->call(0xE0460, tgt, 0, 0, 2, 0, 0);
    if (tgt) {
        if (d2rd32(tgt) == 1) { const uint32_t p = d2rd32(tgt + 0x14); if (p) { d2wr32(p + 0x38, 0); d2wr32(p + 0x34, 0); } }
        d2wr32(tgt + 0xC4, d2rd32(tgt + 0xC4) | 0x80000000u);
    }
    e->call(0xA3CA, 0, 0, 0, 0, 0, tgt, 0x60, 1);
    const int32_t n = (int32_t)e->call(0xAC9A, 0, 0, 0, 0, 0, src, d2rd32(row + 0x13C), (uint32_t)skill, lvl);
    if (n <= 0) return;
    e->call(0x232B0, 0, 0, 0, 0x15, 0, tgt, 0);
    e->call(0x8EA80, 0, game, tgt, 0, 0, 7, d2rd32(game + 0xA8) + (uint32_t)n, 0, 0, 0);
}
