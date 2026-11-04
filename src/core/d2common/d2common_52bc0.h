// 1.13c D2Common+0x52BC0: set a unit's skill level, retail record layout, written from the 1.13c
// disassembly as a rule over E (see d2common_528b0.h). stdcall (unit, skill id, level, bRemove, two
// unused words), ret 0x18.
// Level 0 with bRemove: 0x528B0 (EBX = unit, stack = id, 0), then 0x52480(unit, id).
// Level 0 without it: the node from 0x4F080 (ECX = unit, EDX = id, stack = -1); none adds it with
// 0x52660(unit, id) and asks 0x4F080 again, and a second miss returns. The node's level (+0x28) is set
// to 0 and 0x52480(unit, id) runs.
// Any other level: the first node of the list (unit +0xA8, first at +4, next at +4) whose row word is
// the id with owner (+0x34) -1, else the node 0x52660(unit, id) answers; a node gets the level at
// +0x28, and 0x52480(unit, id) runs either way.
#pragma once
#include <cstdint>

namespace d2c52bc0 {
template <class E> void SetSkillLevel(E& e, uint32_t u, int32_t id, int32_t lvl, int32_t bRemove)
{
    if (lvl == 0) {
        if (bRemove) {
            e.h(0x528B0, 0, 0, 0, u, 0, (uint32_t)id, 0);
            e.h(0x52480, 0, 0, 0, 0, 0, u, (uint32_t)id);
            return;
        }
        uint32_t s = e.h(0x4F080, 0, u, (uint32_t)id, 0, 0, 0xFFFFFFFFu);
        if (!s) {
            e.h(0x52660, 0, 0, 0, 0, 0, u, (uint32_t)id);
            s = e.h(0x4F080, 0, u, (uint32_t)id, 0, 0, 0xFFFFFFFFu);
            if (!s) return;
        }
        e.wr32(s + 0x28, 0);
        e.h(0x52480, 0, 0, 0, 0, 0, u, (uint32_t)id);
        return;
    }
    uint32_t n = 0;
    if (const uint32_t l = e.rd32(u + 0xA8))
        for (n = e.rd32(l + 4); n; n = e.rd32(n + 4))
            if (E::rd16s(e.rd32(n)) == id && e.rd32(n + 0x34) == 0xFFFFFFFFu) break;
    if (!n) n = e.h(0x52660, 0, 0, 0, 0, 0, u, (uint32_t)id);
    if (n) e.wr32(n + 0x28, (uint32_t)lvl);
    e.h(0x52480, 0, 0, 0, 0, 0, u, (uint32_t)id);
}
}
