// D2Game+0xD6E30: place the item the unit holds on its cursor. EAX = item id, stack a0 = game, a1 = unit, a2 = out cell (ret 0xC). Written from the 1.13c disassembly.
// game == 0, unit == 0, inv == 0 and a 0xA9B8 failure assert-exit (not modelled). *out = 0; inv = [unit + 0x60]; 0xA6A6(inv) or 0x37680 (ESI = unit) != 0: return 0.
// Lookup game[0x1720 + (id & 0x7F) * 4] chain through item[0xE4] on item[0xC] == id; absent: *out = 1; item[0] != 4: return 0; item[0x10] != 3: *out = 1.
// 0xD29F0(unit); 0xD2F00(game, item) == 0: 0xD2840 (ESI = item, stack 0x13), return 0. 0xDFFD0 (ESI = unit, stack 1, unit). 0xAA66(item) == 4: 0xD6C80 (ESI = unit, game, item), return 1.
// 0xD4360 (EAX = unit, ECX = game, ESI = item) != 0: return 0. 0xD2610 (EAX = item, stack unit, &local, 0) != 0: 0xAA78(unit, local, item, 0) != 1: *out = 1, return 0;
// 0xA478(item) gating 0xA44E, 0xA550, 0xA63A; 0xD4960 (EAX = 0, ECX = item, EDX = unit, stack game) == 0: return 0; else 0x345A0 (EAX = item, ECX = game, EDX = unit), return 1.
// 0xD2610 == 0: 0xA8D4(item), 0xA9CA(inv, item), 0xAA30(inv, item) all nonzero place the item (0xA9B8, 0xAA54, 0xD4670, 0xA640 gating 0xD47C0, flag edits, 0xA994, 0xA9A6, 0xA640 gating 0xD51A0), return 1;
// otherwise 0xD6590 (EAX = item, ECX = unit, stack game, 1, 0, file, 0x7F0) nonzero returns 1, zero 0xD2840 (ESI = item, stack 0x17) returns 0.
#include "d2game_seam4e.h"
static const uint32_t kFile = 0xB20FB14Cu;   // the source-file string at D2Game+0xFB14C, recorded as a module address
// 1.13c D2Game+0xD6E30
extern "C" uint32_t d2game_d6e30(uint32_t id, uint32_t game, uint32_t unit, uint32_t out, const D2Seam* e)
{
    if (!game || !unit) return 0;
    d2wr32(out, 0);
    const uint32_t inv = d2rd32(unit + 0x60);
    if (!inv) return 0;
    if (e->call(0xA6A6, 0, 0, 0, 0, 0, inv)) return 0;
    if (e->call(0x37680, 0, 0, 0, 0, unit)) return 0;
    uint32_t it = d2rd32(game + 0x1720 + (id & 0x7F) * 4);
    while (it && d2rd32(it + 0xC) != id) it = d2rd32(it + 0xE4);
    if (!it) { d2wr32(out, 1); return 0; }
    if (d2rd32(it) != 4) return 0;
    if (d2rd32(it + 0x10) != 3) { d2wr32(out, 1); return 0; }
    e->call(0xD29F0, 0, 0, 0, 0, 0, unit);
    if (!e->call(0xD2F00, 0, 0, 0, 0, 0, game, it)) { e->call(0xD2840, 0, 0, 0, 0, it, 0x13); return 0; }
    e->call(0xDFFD0, 0, 0, 0, 0, unit, 1, unit);
    if (e->call(0xAA66, 0, 0, 0, 0, 0, it) == 4) { e->call(0xD6C80, 0, 0, 0, 0, unit, game, it); return 1; }
    if (e->call(0xD4360, unit, game, 0, 0, it)) return 0;
    uint32_t local = 0;
    if (e->call(0xD2610, it, 0, 0, 0, 0, unit, (uint32_t)(uintptr_t)&local, 0)) {
        if (e->call(0xAA78, 0, 0, 0, 0, 0, unit, local, it, 0) != 1) { d2wr32(out, 1); return 0; }
        const uint32_t slot = e->call(0xA478, 0, 0, 0, 0, 0, it);
        if (slot) {
            e->call(0xA44E, 0, 0, 0, 0, 0, slot, d2rd32(it), d2rd32(it + 0xC));
            e->call(0xA550, 0, 0, 0, 0, 0, it);
            e->call(0xA63A, 0, 0, 0, 0, 0, it);
        }
        if (!e->call(0xD4960, 0, it, unit, 0, 0, game)) return 0;
        e->call(0x345A0, it, game, unit, 0, 0);
        return 1;
    }
    if (e->call(0xA8D4, 0, 0, 0, 0, 0, it) && e->call(0xA9CA, 0, 0, 0, 0, 0, inv, it) && e->call(0xAA30, 0, 0, 0, 0, 0, inv, it)) {
        if (e->call(0xA478, 0, 0, 0, 0, 0, it)) {
            e->call(0xA44E, 0, 0, 0, 0, 0, e->call(0xA478, 0, 0, 0, 0, 0, it), d2rd32(it), d2rd32(it + 0xC));
            e->call(0xA550, 0, 0, 0, 0, 0, it);
            e->call(0xA63A, 0, 0, 0, 0, 0, it);
        }
        e->call(0xA9B8, 0, 0, 0, 0, 0, inv, it, 2);
        e->call(0xAA54, 0, 0, 0, 0, 0, inv, 0);
        e->call(0xD4670, it, unit, 0, 0, 0);
        if (e->call(0xA640, 0, 0, 0, 0, 0, it, unit)) e->call(0xD47C0, 0, 0, 0, 0, unit, 0);
        d2wr32(it + 0xC4, d2rd32(it + 0xC4) & 0xFFFFFFFDu);
        e->call(0xA4A2, 0, 0, 0, 0, 0, it, 2);
        e->call(0xA92E, 0, 0, 0, 0, 0, it, 0xFF);
        e->call(0xA916, 0, 0, 0, 0, 0, it, 0x2000, 1);
        d2wr32(it + 0xC4, d2rd32(it + 0xC4) & 0xFDFFFFFFu);
        e->call(0xA994, 0, 0, 0, 0, 0, inv, it);
        e->call(0xA9A6, 0, 0, 0, 0, 0, unit, 1);
        if (e->call(0xA640, 0, 0, 0, 0, 0, it, unit)) {
            e->call(0xD51A0, 0, 0, 0, 0, 0, game, unit, 0);
            e->call(0xA9A6, 0, 0, 0, 0, 0, unit, 1);
        }
        return 1;
    }
    if (e->call(0xD6590, it, unit, 0, 0, 0, game, 1, 0, kFile, 0x7F0)) return 1;
    e->call(0xD2840, 0, 0, 0, 0, it, 0x17);
    return 0;
}
