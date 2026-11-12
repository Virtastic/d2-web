// 1.13c D2Common+0x157D0: level preset callback (table entry). ecx = the build record: the slot index at +0x1EC,
// a table id at +0x1F0. When the slot's word at +0x138 + 4 * slot is -1 it is copied to +0xFC + 4 * slot. The id
// selects a 0x9C-byte record of the table at the global 0xA0B38 (count at data tables +0xC5C; an id outside it
// gives a null record, which retail then reads through), and the slot's pair at +8 + 16 * slot takes the record's
// words +0x24 and +0x28. The result is 1.
#pragma once
#include <cstdint>

namespace c157d0 {
template <class E> uint32_t Fill(E& e, uint32_t dt, uint32_t table, uint32_t rec)
{
    uint32_t slot = e.rd32(rec + 0x1EC);
    if (e.rd32(rec + slot * 4 + 0x138) == 0xFFFFFFFFu) e.wr32(rec + slot * 4 + 0xFC, e.rd32(rec + slot * 4 + 0x138));
    const int32_t id = (int32_t)e.rd32(rec + 0x1F0);
    const uint32_t row = (id >= 0 && id < (int32_t)e.rd32(dt + 0xC5C)) ? table + (uint32_t)id * 0x9C : 0;
    slot = e.rd32(rec + 0x1EC);
    e.wr32(rec + slot * 16 + 8, e.rd32(row + 0x24));
    slot = e.rd32(rec + 0x1EC);
    e.wr32(rec + slot * 16 + 0xC, e.rd32(row + 0x28));
    return 1;
}
}
