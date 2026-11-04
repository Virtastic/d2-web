// 1.13c D2Common+0x529E0 (#10506): give a unit its class base skills, retail record layout, written from
// the 1.13c disassembly as a rule over E (see d2common_528b0.h). stdcall (unit), ret 4. T is the value of
// sgptDataTables: +0xBA0 skills.txt count, +0xBC4 charstats rows (0xC4 each), +0xBC8 their count.
// Unless the skill list (unit +0xA8, first node at +4) already holds a node whose row word is 0 with
// owner -1, the unit's class (+0x04) must index a charstats row (an out-of-range class or a null row
// returns at once); 0x52660(unit, 0) adds skill 0, then each of the row's ten signed base-skill words at
// +0xAE that is >= 0 and below the skills count goes through 0x52660(unit, id). Then, with the list
// re-read, a null left skill (+0x08) runs 0x4ED60(unit, 0, -1) and, with the list re-read again, a null
// right skill (+0x0C) runs 0x4ECC0(unit, 0, -1). A null list at either re-read is fatal in retail
// (0x3EA / 0x40F); the rule returns.
#pragma once
#include <cstdint>

namespace d2c529e0 {
template <class E> void InitBaseSkills(E& e, uint32_t T, uint32_t u)
{
    bool have = false;
    if (const uint32_t l = e.rd32(u + 0xA8))
        for (uint32_t n = e.rd32(l + 4); n && !have; n = e.rd32(n + 4))
            have = E::rd16s(e.rd32(n)) == 0 && e.rd32(n + 0x34) == 0xFFFFFFFFu;
    if (!have) {
        const int32_t cls = (int32_t)e.rd32(u + 4);
        if (cls < 0 || cls >= (int32_t)e.rd32(T + 0xBC8)) return;
        const uint32_t row = e.rd32(T + 0xBC4) + (uint32_t)cls * 0xC4;
        if (!row) return;
        e.h(0x52660, 0, 0, 0, 0, 0, u, 0);
        for (uint32_t i = 0; i < 10; i++) {
            const int32_t id = E::rd16s(row + 0xAE + 2 * i);
            if (id >= 0 && id < (int32_t)e.rd32(T + 0xBA0)) e.h(0x52660, 0, 0, 0, 0, 0, u, (uint32_t)id);
        }
    }
    uint32_t l = e.rd32(u + 0xA8);
    if (!l) return;   // fatal 0x3EA
    if (!e.rd32(l + 8)) e.h(0x4ED60, 0, 0, 0, 0, 0, u, 0, 0xFFFFFFFFu);
    l = e.rd32(u + 0xA8);
    if (!l) return;   // fatal 0x40F
    if (!e.rd32(l + 0xC)) e.h(0x4ECC0, 0, 0, 0, 0, 0, u, 0, 0xFFFFFFFFu);
}
}
