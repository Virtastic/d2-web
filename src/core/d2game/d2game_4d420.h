// 1.13c D2Game+0x4D420 (SkillSpawnMissileFan: a line of missiles across the aim direction, centred on the target point):
// ECX = game, EDX = unit; stack = (skill, lvl), ret 8. Written from the 1.13c disassembly as a rule over E (HookEnv).
// A set unit gets unit+0xC4 |= 0x40. Skill row = Skills.txt base dt+0xB98 + 0x23C * skill, for 0 <= skill < dt+0xBA0 and a nonzero row, else 0.
// 0x9FA30 (EAX unit, EBX &tx, stack &ty; ret 4) fills the target point; 0 returns 1. n := 0xAC9A(unit, row+0x138 calc, skill, lvl).
// The unit's own point: types 2, 4, 5 read the static path +0xC / +0x10, other types call 0xA454 / 0xA3B8 (stdcall path), a null path is 0.
// d = (tx - ux, ty - uy) goes through 0x9E490 (ESI &dx, EDX &dy) and 0x9E450 (EBX &dx, EDI &dy), which write it in place.
// Start = (tx - (dx * n) / 2, ty - (dy * n) / 2) (signed, toward zero). Missile = (short)row+0x48, or (short)row+0x4A when 0xAC28(ECX unit) != 1
// and that is >= 0; outside [0, Missiles count dt+0xB6C) returns 0. w := 0xAC9A(unit, row+0x140, ...), 0 means n.
// The 23-dword request block (zeroed): [1] [2] unit, [4] missile, [11] skill, [12] lvl, [17] 0xAC9A(unit, row+0x13C, ...), [7] [8] the point.
// (n - w) / 2 missiles with flags [0] 0x10820, then w with 0x820, then the rest of n with 0x10820, each through 0x6F930 (ECX game, EDX &block),
// the point stepping by d after each one. The result is 1.
#pragma once
#include <cstdint>

namespace g4d420 {
template <class E> uint32_t SkillSpawnMissileFan(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t skill, uint32_t lvl)
{
    // 1.13c D2Game+0x4D420
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t row = e.rd32(dt + 0xB98) + skill * 0x23C;
    if (!row) return 0;
    uint32_t tp[2] = { 0, 0 };   // tx, ty (EBX and stack out-pointers)
    e.ed(0);
    const uint32_t ok = e.h(0x9FA30, unit, 0, 0, e.stk(&tp[0]), 0, e.stk(&tp[1]));
    if (!ok) return 1;
    const int32_t n = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(row + 0x138), skill, lvl);
    const uint32_t ty = e.rd32(unit);
    const bool direct = ty == 2 || ty == 4 || ty == 5;
    const uint32_t path = e.rd32(unit + 0x2C);
    const uint32_t ux = direct ? e.rd32(path + 0xC) : path ? e.h(0xA454, 0, 0, 0, 0, 0, path) : 0;
    const uint32_t uy = direct ? e.rd32(path + 0x10) : path ? e.h(0xA3B8, 0, 0, 0, 0, 0, path) : 0;
    uint32_t d[2] = { tp[0] - ux, tp[1] - uy };
    e.h(0x9E490, 0, 0, e.stk(&d[1]), 0, e.stk(&d[0]));
    e.ed(e.stk(&d[1]));
    e.h(0x9E450, 0, 0, 0, e.stk(&d[0])); e.edi(0x9E450, e.stk(&d[1]));
    int32_t x = (int32_t)(tp[0] - (uint32_t)((int32_t)(d[0] * (uint32_t)n) / 2));
    int32_t y = (int32_t)(tp[1] - (uint32_t)((int32_t)(d[1] * (uint32_t)n) / 2));
    int32_t mid = e.rd16s(row + 0x48);
    if (e.h(0xAC28, 0, unit) != 1) { const int32_t alt = e.rd16s(row + 0x4A); if (alt >= 0) mid = alt; }
    if (mid < 0 || mid >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    int32_t w = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(row + 0x140), skill, lvl);
    if (!w) w = n;
    uint32_t s[0x17] = {};
    s[1] = unit; s[2] = unit; s[4] = (uint32_t)mid; s[0xB] = skill; s[0xC] = lvl;
    s[0x11] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(row + 0x13C), skill, lvl);
    auto run = [&](int32_t cnt, uint32_t flags) {
        s[0] = flags;
        for (int32_t i = 0; i < cnt; i++) {
            s[7] = (uint32_t)x; s[8] = (uint32_t)y;
            e.h(0x6F930, 0, game, e.stk(s)); e.blk(0x6F930, s, 0x17);
            x = (int32_t)((uint32_t)x + d[0]); y = (int32_t)((uint32_t)y + d[1]);
        }
    };
    const int32_t head = (int32_t)((uint32_t)n - (uint32_t)w) / 2;
    run(head, 0x10820);
    run(w, 0x820);
    run((int32_t)((uint32_t)n - (uint32_t)head - (uint32_t)w), 0x10820);
    return 1;
}
}
