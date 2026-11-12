// 1.13c D2Game+0xBD920, transcribed from the 1.13c disassembly: put an item into a client's inventory record.
// Convention: EAX = owner, ECX (unused), ESI = item, stack game, flag (ret 8). A null item returns the EAX it was handed, a null
// owner returns 0. Callees: 0x8FAA0 (ECX = game; plain ret) gives the record, 0 returns 0; 0xD62E0 (EAX = record, ECX = owner, stack
// game, [item+0xc], the address of the game stack word, 0, 0, 0; ret 0x18); flag set: item[+0xc8] |= 0x10, Ordinal thunks 0xA9A6
// (owner, 1), 0xA994 (record, item), 0xA916 (item, 1, 1) whose EAX is the result; flag clear: 0xDFC10 (EBX = the 0xE7E0 callback,
// EDI = game, stack item; ret 4), then 0xE20A0 (ECX = game, EDX = item; plain ret), whose EAX is the result.
#pragma once
#include <cstdint>

namespace rbd920 {
template <class E> uint32_t Do(E& e, uint32_t owner, uint32_t item, uint32_t game, uint32_t flag)
{
    if (!item) return owner;
    if (!owner) return 0;
    const uint32_t rec = e.h(0x8FAA0, 0, game);
    if (!rec) return 0;
    uint32_t slot = game;
    const uint32_t w[2] = { 0, 0 };
    e.h(0xD62E0, rec, owner, 0, 0, 0, game, e.rd32(item + 0xc), (uint32_t)(uintptr_t)&slot, 0);
    e.x(0xD62E0, w, 2);
    if (flag) {
        e.wr32(item + 0xc8, e.rd32(item + 0xc8) | 0x10);
        e.h(0xA9A6, 0, 0, 0, 0, 0, owner, 1);
        e.h(0xA994, 0, 0, 0, 0, 0, rec, item);
        return e.h(0xA916, 0, 0, 0, 0, 0, item, 1, 1);
    }
    e.h(0xDFC10, 0, 0, 0, 0x6FC2E7E0u, 0, item);
    e.edi(0xDFC10, game);
    return e.h(0xE20A0, 0, game, item);
}
}
