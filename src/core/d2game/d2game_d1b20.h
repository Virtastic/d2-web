// 1.13c D2Game+0xD1B20, transcribed from the 1.13c disassembly: find a quiver in an owner's inventory that a missile item can refill.
// Convention: EAX = item, stack owner, out (ret 8). 0xA430 (item, 0x16) == 0 or no inventory ([owner+0x60]) return 0. The list from
// 0xA3DC (inv), stepped with 0xA46C / 0xA4CC, is searched for an entry with 0xAA66 == 0x12, a zero low byte of 0xA6C4, 0xD16B0
// (EBX = entry, EDI = item; plain ret) != 0 and a 0xA42A (entry, 0x46, 0) count (negative read as 0) below its 0xAA06 maximum: the
// room left goes to *out and the entry is returned. None: 0.
#pragma once
#include <cstdint>

namespace rd1b20 {
template <class E> uint32_t Do(E& e, uint32_t item, uint32_t owner, uint32_t out)
{
    if (!e.h(0xA430, 0, 0, 0, 0, 0, item, 0x16)) return 0;
    const uint32_t inv = e.rd32(owner + 0x60);
    if (!inv) return 0;
    for (uint32_t n = e.h(0xA3DC, 0, 0, 0, 0, 0, inv); n;) {
        const uint32_t it = e.h(0xA46C, 0, 0, 0, 0, 0, n);
        n = e.h(0xA4CC, 0, 0, 0, 0, 0, n);
        if (!it) continue;
        if (e.h(0xAA66, 0, 0, 0, 0, 0, it) != 0x12) continue;
        if (e.h(0xA6C4, 0, 0, 0, 0, 0, it) & 0xFF) continue;
        const uint32_t ok = e.h(0xD16B0, 0, 0, 0, it, 0);
        e.edi(0xD16B0, item);
        if (!ok) continue;
        int32_t have = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, it, 0x46, 0);
        if (have < 0) have = 0;
        const uint32_t mx = e.h(0xAA06, 0, 0, 0, 0, 0, it);
        if (have >= (int32_t)mx) continue;
        e.wr32(out, mx - (uint32_t)have);
        return it;
    }
    return 0;
}
}
