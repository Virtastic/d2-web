// 1.13c D2Game+0xBD880, transcribed from the 1.13c disassembly: put an item into its owner's inventory record.
// Convention: EAX = item, EBX = game, EDI = owner, one stack word flag (ret 4). A null game, owner or item is a fatal exit.
// Callees: 0xD62E0 (EAX = 0, ECX = owner, stack game, [item+0xc], the address of a local word, 0, 0, 0; ret 0x18);
// flag set: item[+0xc8] |= 0x10, then Ordinal thunks 0xA9A6 (owner, 1), 0xA994 ([owner+0x60], item), 0xA916 (item, 1, 1), whose EAX is
// the result; flag clear: 0xEE70 (EAX = game, ESI = item; plain ret), whose EAX is the result.
#pragma once
#include <cstdint>
#include <cstdlib>

namespace rbd880 {
template <class E> uint32_t Do(E& e, uint32_t item, uint32_t game, uint32_t owner, uint32_t flag)
{
    if (!game || !owner || !item) abort();
    uint32_t slot = 0;
    const uint32_t w[2] = { 0, 0 };
    e.h(0xD62E0, 0, owner, 0, 0, 0, game, e.rd32(item + 0xc), (uint32_t)(uintptr_t)&slot, 0);
    e.x(0xD62E0, w, 2);
    if (flag) {
        e.wr32(item + 0xc8, e.rd32(item + 0xc8) | 0x10);
        e.h(0xA9A6, 0, 0, 0, 0, 0, owner, 1);
        e.h(0xA994, 0, 0, 0, 0, 0, e.rd32(owner + 0x60), item);
        return e.h(0xA916, 0, 0, 0, 0, 0, item, 1, 1);
    }
    return e.h(0xEE70, game, 0, 0, 0, item);
}
}
