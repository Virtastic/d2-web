// Monster-kind flag check: EBX = a state block (flag byte +4), ESI = unit; plain ret. With bit 2 of the flag byte set and a
// type-1 unit whose class (+4) is not negative: a class below the monster-stats count (data tables +0xA80) is replaced by its
// row's base id (the signed short at row +2; rows of 0x1A8 bytes at data tables +0xA78). If the result is a valid class equal
// to 0x4E, the unit query (unit, 1) runs, and a zero answer sets bit 3 of the flag byte. dt = the data tables.
#pragma once
#include <cstdint>

namespace d2g771d0 {
template <class E> void Run(E& e, uint32_t blk, uint32_t unit, uint32_t dt)
{
    // 1.13c D2Game+0x771D0
    if (!(e.rd8(blk + 4) & 4) || !unit || e.rd32(unit) != 1) return;
    int32_t cls = (int32_t)e.rd32(unit + 4);
    if (cls < 0) return;
    const int32_t count = (int32_t)e.rd32(dt + 0xA80);
    if (cls < count) {
        const uint32_t row = (uint32_t)cls * 0x1A8 + e.rd32(dt + 0xA78);
        if (row) cls = e.rd16s(row + 2);
    }
    if (cls < 0 || cls >= count || cls != 0x4E) return;
    if (!e.h(0xA68E, 0, 0, 0, 0, 0, unit, 1)) e.wr8(blk + 4, e.rd8(blk + 4) | 8);
}
}
