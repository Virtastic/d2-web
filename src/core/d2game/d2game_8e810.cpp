// 1.13c D2Game+0x8E810 -- queue an event node on a game's per-unit-type list.
// EAX = unit (may be null), EDI = game, EBX = event type (signed, skipped when >= 15); stack (ret 0xC): a1 -> node+0x2C, a2 -> node+0x14, a3 -> node+0x18.
// The node comes from 0x8E680 (EAX = game, ECX = unit) and is pushed on the list head at game->+0xB8 + table[type]*4 + 0xA04
// (table 0x6FD1A3DC = {0,1,3,2,4,-1,0,0}; a null unit uses entry 6).
#include "d2game_8d6_common.h"
struct D2Game8E810Env {
    uint32_t (*allocNode)(uint32_t game, uint32_t unit);   // 0x8E680 (EAX = game, ECX = unit)
};
// 1.13c D2Game+0x8E810
void d2game_port_8e810(uint32_t unit, uint32_t game, int32_t type, uint32_t a1, uint32_t a2, uint32_t a3, const D2Game8E810Env* env)
{
    static const uint32_t kTable[8] = {0, 1, 3, 2, 4, 0xFFFFFFFFu, 0, 0};   // .rdata 0x6FD1A3DC
    if (type >= 0xF) return;
    const uint32_t node = env->allocNode(game, unit);
    d2g8::w8(node, 2, d2g8::r8(node, 2) | 4);
    d2g8::w32(node, 0x14, a2);
    d2g8::w8(node, 0, (uint8_t)type);
    d2g8::w32(node, 4, 0xFFFFFFFFu);
    d2g8::w32(node, 0x18, a3);
    d2g8::w32(node, 0x2C, a1);
    const uint32_t kind = unit ? d2g8::r32(unit, 0) : 6;
    const uint32_t slot = d2g8::r32(game, 0xB8) + kTable[kind & 7] * 4 + 0xA04;
    const uint32_t head = d2g8::r32(slot, 0);
    d2g8::w32(node, 0x1C, head);
    d2g8::w32(node, 0x20, 0);
    if (head) d2g8::w32(head, 0x20, node);
    d2g8::w32(slot, 0, node);
}
