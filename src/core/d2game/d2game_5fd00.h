// 1.13c D2Game+0x5FD00: object operate callback "last portal" (table entry). ecx = the operate block. With
// the quest record 0x24: unless its data word +0x9C is 2: 0. The player's quest flags (player data 0xA4C0,
// word +0x10 + 4 * the game's difficulty byte +0x6D) must have quest 0x28 bit 0xD (0xA568), else the player
// only hears the sound 0x13 (0xDFFD0: esi = player; stack 0x13, player). Then the level change to 0x6D is
// sent (0x9E170: eax = player, edi = 0x6D; stack: game, 0), the player's client is read (player data +0x9C,
// only for a unit of type 0), the player data is read again and the record's game (+4) is saved (0x2DE30: eax
// = game). With a client and 0x37680 (esi = player) answering 0: the player's +0x64 = -1, +0x68 = 6, byte
// +0x6C = 0, player data +0x4C = 1, the two-byte message 0x61 0x07 goes to the client (0x8A3E0: eax = client;
// stack: bytes, 2) and quest bit 0xA of the record's quest (+0xE0) is set in the flags (0xA57A). The result
// is 0.
#pragma once
#include <cstdint>
#include "d2game_objinit.h"

namespace g5fd00 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t game = e.rd32(op);
    const uint32_t q = gobjinit::Find(e, op, 0x24);
    if (e.rd32(e.rd32(q + 0x18) + 0x9C) != 2) return 0;
    uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(op + 8));
    const uint32_t flags = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    if (!e.h(0xA568, 0, 0, 0, 0, 0, flags, 0x28, 0xD)) {
        const uint32_t pl = e.rd32(op + 8);
        e.h(0xDFFD0, 0, 0, 0, 0, pl, 0x13, pl);
        return 0;
    }
    e.h(0x9E170, e.rd32(op + 8), 0, 0, 0, 0, e.rd32(op), 0);
    e.j(0x9E170, 0x6D);
    uint32_t pl = e.rd32(op + 8);
    const uint32_t client = (pl && e.rd32(pl) == 0) ? e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, pl) + 0x9C) : 0;
    pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(op + 8));
    e.h(0x2DE30, e.rd32(q + 4));
    if (!client) return 0;
    if (e.h(0x37680, 0, 0, 0, 0, e.rd32(op + 8))) return 0;
    pl = e.rd32(op + 8);
    e.wr32(pl + 0x64, 0xFFFFFFFFu);
    e.wr32(pl + 0x68, 6);
    e.wr8(pl + 0x6C, 0);
    e.wr32(pd + 0x4C, 1);
    uint32_t msg = 0x0761;   // bytes 0x61, 0x07
    e.h(0x8A3E0, client, 0, 0, 0, 0, (uint32_t)(uintptr_t)&msg, 2);
    e.blk(0x8A3E0, &msg, 1);
    e.h(0xA57A, 0, 0, 0, 0, 0, flags, e.rd32(q + 0xE0), 0xA);
    return 0;
}
}
