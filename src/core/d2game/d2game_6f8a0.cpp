// 1.13c D2Game+0x6F8A0 -- insert a unit into the per-type server list. EAX = unit (null returns at once), EDI = game, stack word = stamp (ret 4).
// Stamps unit+0xC, clears flag bits 1 and 3 of unit+0xC4, detaches an old node at unit+0xDC (0x8E880), takes a fresh node from 0x8E680,
// initialises it (flags |= 4, owner -1, +0x14/+0x18/+0x2C zero) and pushes it on the head of the doubly linked list at
// [game+0xB8] + 0xA04 + 4 * typeTable[unit type].
#include <cstring>
#include "d2game_6f8a0.h"
namespace {
uint32_t rd(uint32_t b, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(b + o), 4); return v; }
void wr(uint32_t b, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(b + o), &v, 4); }
}  // namespace
// 1.13c D2Game+0x6F8A0
void d2game_port_6f8a0(uint32_t unit, uint32_t game, uint32_t stamp, const D2Game6F8A0Env* env)
{
    if (!unit) return;
    env->stamp(unit);
    wr(unit, 0xc, stamp);
    wr(unit, 0xc4, rd(unit, 0xc4) & 0xfffffff5u);
    if (rd(unit, 0xdc)) env->detach(game, unit);
    wr(unit, 0xdc, 0);
    const uint32_t rec = env->node(game, unit);
    *(uint8_t*)(uintptr_t)(rec + 2) |= 4;
    *(uint8_t*)(uintptr_t)rec = 0;
    wr(rec, 4, 0xffffffffu);
    wr(rec, 0x14, 0); wr(rec, 0x18, 0); wr(rec, 0x2c, 0);
    const uint32_t head = rd(game, 0xb8) + 0xa04 + env->typeSlot(rd(unit, 0)) * 4;
    const uint32_t first = rd(head, 0);
    wr(rec, 0x1c, first);
    wr(rec, 0x20, 0);
    if (first) wr(first, 0x20, rec);
    wr(head, 0, rec);
}
