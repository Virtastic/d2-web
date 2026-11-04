// D2Game+0xD3280 -- EAX = item, ECX = unit, stack a0 = game (ret 4). Written from the 1.13c disassembly; callees are the D2Seam.
// Any of game, unit, item null, [item] != 4 or [unit + 0x60] == 0 returns at once. Else on the item's mode [item + 0x10]:
//  0: 0xA6C4(item) = r; 0xA958(item, r); if [unit] == 0: pd = 0xA4C0(unit), ebx = [pd + 0x9C], ebx != 0: local = 0, 0xAA5A(&local, 0x20, 1),
//     0x8C810 (EAX = item, EBX = ebx, stack local); then 0xD2D90 (EAX = unit, stack game, 0).
//  1: 0xA98E(inv, item) = r; 0xD8970 (ECX = game, EDX = unit, stack r, 1, &local).
//  4: 0xD2C70 (EAX = unit, stack game).   Other modes do nothing.
#include "d2game_seam4e.h"
// 1.13c D2Game+0xD3280
extern "C" void d2game_d3280(uint32_t item, uint32_t unit, uint32_t game, const D2Seam* e)
{
    if (!game || !unit || !item || d2rd32(item) != 4) return;
    const uint32_t inv = d2rd32(unit + 0x60);
    if (!inv) return;
    const uint32_t mode = d2rd32(item + 0x10);
    if (mode == 0) {
        const uint32_t r = e->call(0xA6C4, 0, 0, 0, 0, 0, item);
        e->call(0xA958, 0, 0, 0, 0, 0, item, r);
        if (d2rd32(unit) == 0) {
            const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
            const uint32_t ebx = d2rd32(pd + 0x9C);
            if (ebx) {
                uint32_t local = 0;
                e->call(0xAA5A, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)&local, 0x20, 1);
                e->call(0x8C810, item, 0, 0, ebx, 0, local);
            }
        }
        e->call(0xD2D90, unit, 0, 0, 0, 0, game, 0);
    } else if (mode == 1) {
        const uint32_t r = e->call(0xA98E, 0, 0, 0, 0, 0, inv, item);
        uint32_t out = 0;
        e->call(0xD8970, 0, game, unit, 0, 0, r, 1, (uint32_t)(uintptr_t)&out);
    } else if (mode == 4) {
        e->call(0xD2C70, unit, 0, 0, 0, 0, game);
    }
}
