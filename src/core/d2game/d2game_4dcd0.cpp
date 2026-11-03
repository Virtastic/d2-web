// D2Game+0x4DCD0 -- SkillSpawnChainedTarget: pick the next target of a chained skill, spawn its missile and advance the chain.
// ECX = game, EDX = unit, stack (skill id, arg1), ret 8. Skill id outside 0..dt +0xBA0 or row address 0: 0.
// own = 0xAABA(unit), none: 0; ac1 = 0xAC9A(unit, row +0x64, skill, arg1); n = 0xAD12(own), zero: 0; b = 0xAD3C(own); a = 0xACB2(own);
// tgt = 0xE03A0(EAX = a, ECX = game, EDX = b); none: 0xA0F40(EAX = 3, ECX = b, EDX = ac1; game, unit, 0, 0), none: return 1. 0xACD0(unit, tgt).
// 0x9FA30(EAX = unit, ECX = game, EBX = &x; &y) (result ignored); missile = signed word row +0x48, or (when 0xAC28(ECX = unit) != 1) the word at row +0x4A if not negative;
// outside 0..dt +0xB6C: 0. ac2 = 0xAC9A(unit, row +0x13C, skill, arg1). Request (23 dwords): [0] 0x21, [1] unit, [4] missile, [5] 0x1100(unit), [6] 0x1130(unit),
// [7] x, [8] y, [11] skill, [12] arg1, [21] D2Game+0x4C190, [22] ac2: 0x6F930(ECX = game, EDX = request). 0xAC94(own, n - 1); n - 1 <= 0: 0.
// u = 0xA0F40(EAX = 3, ECX = tgt id (tgt +0xC, -1 without), EDX = ac1; game, unit, 0, 0), none: 1. 0xACDC(own, u +0), 0xAD0C(own, u +0xC), 0xA35D0(EAX = unit; 0x1EC0(EAX = skill)). Returns 1.
#include "d2game_seam4e.h"
// 1.13c D2Game+0x4DCD0
extern "C" uint32_t d2game_4dcd0(uint32_t game, uint32_t unit, int32_t skill, uint32_t arg1, uint32_t dt, const D2Seam* e)
{
    if (skill < 0 || skill >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = (uint32_t)skill * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return 0;
    const uint32_t own = e->call(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!own) return 0;
    const uint32_t ac1 = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x64), (uint32_t)skill, arg1);
    const uint32_t n = e->call(0xAD12, 0, 0, 0, 0, 0, own);
    if (!n) return 0;
    const uint32_t b = e->call(0xAD3C, 0, 0, 0, 0, 0, own);
    const uint32_t a = e->call(0xACB2, 0, 0, 0, 0, 0, own);
    uint32_t tgt = e->call(0xE03A0, a, game, b);
    if (!tgt) {
        tgt = e->call(0xA0F40, 3, b, ac1, 0, 0, game, unit, 0, 0);
        if (!tgt) return 1;
    }
    e->call(0xACD0, 0, 0, 0, 0, 0, unit, tgt);
    uint32_t x = 0, y = 0;
    e->call(0x9FA30, unit, game, 0, (uint32_t)(uintptr_t)&x, 0, (uint32_t)(uintptr_t)&y);
    int32_t id = (int16_t)d2rd16(row + 0x48);
    if (e->call(0xAC28, 0, unit) != 1) {
        const int16_t w = (int16_t)d2rd16(row + 0x4A);
        if (w >= 0) id = w;
    }
    if (id < 0 || id >= (int32_t)d2rd32(dt + 0xB6C)) return 0;
    const uint32_t ac2 = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x13C), (uint32_t)skill, arg1);
    uint32_t q[23] = {};
    q[0] = 0x21; q[1] = unit;
    q[5] = e->call(0x1100, 0, unit);
    q[6] = e->call(0x1130, 0, unit);
    q[4] = (uint32_t)id; q[7] = x; q[8] = y; q[11] = (uint32_t)skill; q[12] = arg1;
    q[21] = 0xB2000000u | 0x4C190u; q[22] = ac2;
    e->call(0x6F930, 0, game, (uint32_t)(uintptr_t)q);
    e->blk(0x6F930, q, 23);
    const int32_t m = (int32_t)n - 1;
    e->call(0xAC94, 0, 0, 0, 0, 0, own, (uint32_t)m);
    if (m <= 0) return 0;
    const uint32_t tid = tgt ? d2rd32(tgt + 0xC) : 0xFFFFFFFFu;
    const uint32_t u = e->call(0xA0F40, 3, tid, ac1, 0, 0, game, unit, 0, 0);
    if (!u) return 1;
    e->call(0xACDC, 0, 0, 0, 0, 0, own, d2rd32(u));
    e->call(0xAD0C, 0, 0, 0, 0, 0, own, d2rd32(u + 0xC));
    const uint32_t r = e->call(0x1EC0, (uint32_t)skill);
    e->call(0xA35D0, unit, 0, 0, 0, 0, r);
    return 1;
}
