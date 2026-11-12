// 1.13c D2Game+0x332F0: object init callback "trapped soul" (table entry). eax as entered, ecx = the init block
// ([0] game, [4] the object). No object: eax is untouched. An object in a mode other than 1 and 2 (+0x10): the
// mode comes back. Otherwise an event of type 9 is queued 35 frames ahead (0x8EA80: game, object, 9, frame + 35,
// 0, 0, 0) and that call's result is the result.
#pragma once
#include <cstdint>

namespace g332f0 {
template <class E> uint32_t Init(E& e, uint32_t eax, uint32_t op)
{
    const uint32_t obj = e.rd32(op + 4);
    if (!obj) return eax;
    const uint32_t mode = e.rd32(obj + 0x10);
    if (mode != 2 && mode != 1) return mode;
    const uint32_t game = e.rd32(op);
    return e.h(0x8EA80, 0, game, obj, 0, 0, 9, e.rd32(game + 0xA8) + 0x23, 0, 0, 0);
}
}
