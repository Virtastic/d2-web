// D2Game+0x4B100 -- queue a unit on the owner's list unless it is already listed.
// EBX = owner (+0x1A8 saved handle, +0x1B4 list handle), stack unit, ret 4. Callees: 0x4AD10 (ESI = owner, stack 0),
// 0xDF620 (ESI = unit, stack saved), thunk 0xA55C (list handle, &array, &count), 0x7B0A0 (saved, unit, owner), fatal path 0xA2DA/0xA2C2/0x22A3C.
#include "d2game_seam4e.h"
// 1.13c D2Game+0x4B100
extern "C" void d2game_4b100(uint32_t owner, uint32_t unit, const D2Seam* e)
{
    const uint32_t saved = d2rd32(owner + 0x1A8);
    if (unit == e->call(0x4AD10, 0, 0, 0, 0, owner, 0)) return;
    const uint32_t y = e->call(0xDF620, 0, 0, 0, 0, unit, saved);
    if (!y) return;
    const uint32_t list = d2rd32(owner + 0x1B4);
    if (!list) { e->call(0xA2DA, 0, 0, 0, 0, 0, 0x827); e->call(0x22A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu); return; }
    uint32_t arr = 0, cnt = 0;
    e->call(0xA55C, 0, 0, 0, 0, 0, list, (uint32_t)(uintptr_t)&arr, (uint32_t)(uintptr_t)&cnt);
    for (uint32_t i = 0; i < cnt; i++) {
        const uint32_t v = d2rd32(arr + 4 * i);
        if (!v) { e->call(0xA2DA, 0, 0, 0, 0, 0, 0x82C); e->call(0x22A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu); return; }
        if (v == y) return;
    }
    e->call(0x7B0A0, 0, 0, 0, 0, 0, saved, unit, owner);
}
