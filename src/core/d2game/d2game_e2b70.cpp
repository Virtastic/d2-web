// 1.13c D2Game+0xE2B70 -- quest node "is the quest active" check (ECX = quest node, EAX at entry only matters for the early return); written from the 1.13c disassembly.
// A node that is not in state 5 and has byte +9 set answers 1 (low byte of the entry EAX replaced). Else the quest record 0x33F30(EAX game, ECX node +0x10) gives its
// check callback (record +0xF0, ECX record); its low byte is kept as BL. The game's list holder (game +0x10F4, first word = head, next at +0xF4) is searched for the first
// kind 0x11 entry: its callback (entry +0xF0) answers, 0 only when both answers are 0, else 1. No such entry: the holder pointer with its low byte replaced by BL.
// The IsBadCodePtr error exits of retail are not modelled.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fe2b70(HookEnv& e, uint32_t eax, uint32_t node)
{
    if (e.rd8(node + 0xC) != 5 && e.rd8(node + 9)) return (eax & ~0xFFu) | 1;
    const uint32_t q = e.h(0x33F30, e.rd32(node + 4), e.rd32(node + 0x10));
    uint32_t bl = 0;
    if (q) bl = e.h(e.rd32(q + 0xF0) & 0xFFFFFF, 0, q) & 0xFF;
    const uint32_t holder = e.rd32(e.rd32(node + 4) + 0x10F4);
    if (!holder) return bl;
    for (uint32_t s = e.rd32(holder); s; s = e.rd32(s + 0xF4)) {
        if (e.rd32(s) == 0x11) {
            const uint32_t al = e.h(e.rd32(s + 0xF0) & 0xFFFFFF, 0, s) & 0xFF;
            return (al == 0 && bl == 0) ? 0 : 1;
        }
    }
    return (holder & ~0xFFu) | bl;
}
}
