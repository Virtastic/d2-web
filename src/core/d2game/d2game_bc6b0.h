// 1.13c D2Game+0xBC6B0, transcribed from the 1.13c disassembly: find a stack in a unit's inventory that an item can top up.
// Convention: ECX = unit, EBX = item, stack out (ret 4). *out = 1 first; no inventory ([unit+0x60]), 0xA940 (item) == 0 or
// 0xAA48 (item) == 0 return 0. With the item's 0xAA06 maximum, the 0xA976 (inv, item, prev) list and then the 0xA8B0 list are
// walked; an entry with 0xA88C (item, entry) != 0 whose 0xA42A (entry, 0x46, 0) count is below its own 0xAA06 maximum (signed)
// writes the room left to *out and is returned. None: *out = the last maximum read, 0 returned. All callees are Ordinal thunks.
#pragma once
#include <cstdint>

namespace rbc6b0 {
template <class E> uint32_t Do(E& e, uint32_t unit, uint32_t item, uint32_t out)
{
    e.wr32(out, 1);
    const uint32_t inv = e.rd32(unit + 0x60);
    if (!inv) return 0;
    if (!e.h(0xA940, 0, 0, 0, 0, 0, item)) return 0;
    if (!e.h(0xAA48, 0, 0, 0, 0, 0, item)) return 0;
    uint32_t mx = e.h(0xAA06, 0, 0, 0, 0, 0, item);
    const uint32_t lists[2] = { 0xA976, 0xA8B0 };
    for (uint32_t list : lists)
        for (uint32_t s = e.h(list, 0, 0, 0, 0, 0, inv, item, 0); s; s = e.h(list, 0, 0, 0, 0, 0, inv, item, s)) {
            if (!e.h(0xA88C, 0, 0, 0, 0, 0, item, s)) continue;
            const int32_t have = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, s, 0x46, 0);
            mx = e.h(0xAA06, 0, 0, 0, 0, 0, s);
            if (have < (int32_t)mx) { e.wr32(out, mx - (uint32_t)have); return s; }
        }
    e.wr32(out, mx);
    return 0;
}
}
