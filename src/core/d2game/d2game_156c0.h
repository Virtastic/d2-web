// 1.13c D2Game+0x156C0: AI init callback "bone wall lifetime" (table entry). ecx = the unit, stack: the AI
// parameter block ([0] the AI state, [0x1C] the monster's stats record) (ret 4). dt = the data tables.
// No stats record: 0. The record's skill id (signed word at +0x170) at or under zero: nothing is written and the
// result is the record pointer with its low word replaced by that id. Otherwise the state's word at +0x14
// becomes the unit's word at +0xA8 plus the skill row's word at +0x14C (row = table at dt+0xB98, 0x23C bytes
// each, count at dt+0xBA0); an id past the table or a null row adds 0. The result is the amount added.
#pragma once
#include <cstdint>

namespace g156c0 {
template <class E> uint32_t Init(E& e, uint32_t dt, uint32_t unit, uint32_t p)
{
    const uint32_t st = e.rd32(p), rec = e.rd32(p + 0x1C);
    if (!rec) return 0;
    const int32_t id = e.rd16s(rec + 0x170);
    if (id <= 0) return (rec & 0xFFFF0000u) | ((uint32_t)id & 0xFFFFu);
    uint32_t add = 0;
    if (id < (int32_t)e.rd32(dt + 0xBA0)) {
        const uint32_t row = e.rd32(dt + 0xB98) + (uint32_t)id * 0x23C;
        if (row) add = e.rd32(row + 0x14C);
    }
    e.wr32(st + 0x14, e.rd32(unit + 0xA8) + add);
    return add;
}
}
