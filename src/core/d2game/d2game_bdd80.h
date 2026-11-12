// 1.13c D2Game+0xBDD80, transcribed from the 1.13c disassembly: place an item for an owner, through a client's trade list when it has one.
// Convention: ECX = game, EAX = item, stack owner, other, mode (ret 0xc).
// 0x8F5C0 (EAX = a local word's address, ECX = game, ESI = owner ? [owner+4] : -1; plain ret) gives the record. When [rec+0xc] != 0
// and mode == 2, the chain from [rec+8] (next at +8) is searched for [node+4] == (other ? [other+0xc] : -1); a match with a list
// [node] walks it with the Ordinal thunks 0xA3DC / 0xA46C / 0xA4CC, and an entry equal to the item ends in 0xBD920 (EAX = owner,
// ECX = other, ESI = item, stack game, 1). Otherwise 0xBCAA0 (EAX = owner, EBX = game, stack item) answers when its low byte is set,
// else 0xBD880 (EAX = item, EBX = game, EDI = owner, stack 1). The EAX of the last callee is the result.
#pragma once
#include <cstdint>

namespace rbdd80 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t item, uint32_t owner, uint32_t other, uint32_t mode)
{
    uint32_t slot = 0;
    const uint32_t rec = e.h(0x8F5C0, (uint32_t)(uintptr_t)&slot, game, 0, 0, owner ? e.rd32(owner + 4) : 0xFFFFFFFFu);
    if (e.rd32(rec + 0xc) && mode == 2) {
        const uint32_t key = other ? e.rd32(other + 0xc) : 0xFFFFFFFFu;
        uint32_t n = e.rd32(rec + 8);
        while (n && e.rd32(n + 4) != key) n = e.rd32(n + 8);
        if (n && e.rd32(n)) {
            for (uint32_t it = e.h(0xA3DC, 0, 0, 0, 0, 0, e.rd32(n)); it;) {
                const uint32_t u = e.h(0xA46C, 0, 0, 0, 0, 0, it);
                const uint32_t nx = e.h(0xA4CC, 0, 0, 0, 0, 0, it);
                if (u == item) return e.h(0xBD920, owner, other, 0, 0, item, game, 1);
                it = nx;
            }
        }
    }
    const uint32_t r = e.h(0xBCAA0, owner, 0, 0, game, 0, item);
    if (r & 0xFF) return r;
    const uint32_t v = e.h(0xBD880, item, 0, 0, game, 0, 1);
    e.edi(0xBD880, owner);
    return v;
}
}
