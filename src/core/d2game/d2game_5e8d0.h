// 1.13c D2Game+0x5E8D0: object init callback "quest giver's outside spot" (table entry). ecx = the init block
// ([0] game, [4] the object). Without the quest record 0x21 (d2game_objinit.h): 0. A record with a clear flag
// byte (+0x09) is left alone (the result is the record). Otherwise the quest data's word at +0xF8 becomes the
// object's id (-1 without an object); with the data's byte at +0xA4 set the result is again the record, else an
// event is queued (0x8EA80: ecx = the record's game (+0x04), edx = the object; stack: 7, the game's frame
// (+0xA8) plus 0x19, 0, 0, 0) and its answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace g5e8d0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0x21);
    if (!q) return 0;
    if (!e.rd8(q + 9)) return q;
    const uint32_t d = e.rd32(q + 0x18);
    e.wr32(d + 0xF8, gobjinit::ObjId(e, e.rd32(op + 4)));
    if (e.rd8(d + 0xA4)) return q;
    const uint32_t game = e.rd32(q + 4);
    return e.h(0x8EA80, 0, game, e.rd32(op + 4), 0, 0, 7, e.rd32(game + 0xA8) + 0x19, 0, 0, 0);
}
}
