// D2Game+0xD9BF0 -- use an item (potion / scroll / tome / key) by id. EAX = item id, ECX = unit, EDX = game, stack a0, a1 (handed to 0xAD0F0), pErr (ret 0xC).
// Written from the 1.13c disassembly. game / unit == 0 are fatal (Fog 0xA2DA / 0xA2C2, exit) and not modelled.
// *pErr = 0 first; no item with the id -> *pErr = 1; item type 4 only; a nonzero 0xA6A6(inventory) -> 0; item mode [0x10] != 0 or 0xA886 == 0 -> *pErr = 1.
// 0xAD0F0 != 0 consumes a stack-count item (type 0x12 quiver: stat 0x46 minus one; else the 0xD17B0 slot's stack count minus one) and returns 1;
// 0 falls through to the item record's code word at +0x80 ("ass ", "xyz ", "tr2 ", "toa ").
#include "d2game_seam4e.h"
// 1.13c D2Game+0xD9BF0
extern "C" uint32_t d2game_d9bf0(uint32_t id, uint32_t unit, uint32_t game, uint32_t a0, uint32_t a1, uint32_t pErr, const D2Seam* e)
{
    if (!game || !unit) return 0;
    d2wr32(pErr, 0);
    e->call(0xD29F0, 0, 0, 0, 0, 0, unit);
    uint32_t it = d2rd32(game + 0x1720 + (id & 0x7F) * 4);
    while (it && d2rd32(it + 0xC) != id) it = d2rd32(it + 0xE4);
    if (!it) { d2wr32(pErr, 1); return 0; }
    if (d2rd32(it) != 4) return 0;
    if (e->call(0xA6A6, 0, 0, 0, 0, 0, d2rd32(unit + 0x60))) return 0;
    if (d2rd32(it + 0x10) != 0 || !e->call(0xA886, 0, 0, 0, 0, 0, it)) { d2wr32(pErr, 1); return 0; }
    const bool quiver = e->call(0xAA66, 0, 0, 0, 0, 0, it) == 0x12;
    if (quiver && (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, it, 0x46, 0) <= 0) return 0;
    if (e->call(0x70D60, 0, unit, 0, 0, 0, game)) return 0;
    if (e->call(0xAD0F0, 0, 0, 0, 0, 0, game, it, a0, a1)) {
        if (quiver) {
            const uint32_t idx = e->call(0xD17B0, 0, 0, 0, 0, it);
            if (idx == 0xFFFFFFFFu) return 1;
            if ((int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, it, 0x46, 0) <= 0) return 1;
            e->call(0xA4D2, 0, 0, 0, 0, 0, it, 0x46, 0xFFFFFFFFu, 0);
            const uint32_t pd = d2rd32(unit) == 0 ? d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C) : 0;
            const uint32_t v = e->call(0xA42A, 0, 0, 0, 0, 0, it, 0x46, 0);
            e->call(0x8B010, it, 0, 0, 0, 0, pd, 1, 0x46, v);
            const uint32_t ty = d2rd32(it), iid = d2rd32(it + 0xC);
            const uint32_t pd2 = d2rd32(unit) == 0 ? d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C) : 0;
            e->call(0x8C3B0, 0x7C, ty & 0xFF, iid, 0, 0, pd2);
            e->call(0xD8DE0, unit, 0, 0, 0, 0, idx);
            return 1;
        }
        const uint32_t idx = e->call(0xD17B0, 0, 0, 0, 0, it);
        if (idx != 0xFFFFFFFFu) {
            const uint32_t t = e->call(0xA3D0, 0, unit, idx, 0, 0, 0xFFFFFFFFu);
            if (t) {
                int32_t s = (int32_t)e->call(0xA8E0, 0, 0, 0, 0, 0, t) - 1;
                if (s <= 0) {
                    if (e->call(0xA3B2, 0, 0, 0, 0, 0, unit) == t) e->call(0xA2100, 0, unit, 0, 0, 0, 0, 0xFFFFFFFFu);
                    s = 0;
                }
                e->call(0xA862, 0, 0, 0, 0, 0, t, (uint32_t)s);
                const uint32_t ty = d2rd32(unit), uid = d2rd32(unit + 0xC);
                const uint32_t pd = ty == 0 ? d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C) : 0;
                e->call(0x8D570, ty, idx, uid, 0, pd, (uint32_t)s);
            }
        }
        e->call(0xD29F0, 0, 0, 0, 0, 0, unit);
        e->call(0xD39B0, it, unit, 0, 0, 0, game);
        return 1;
    }
    const uint32_t rec = e->call(0xA53E, 0, 0, 0, 0, 0, d2rd32(it + 4));
    const uint32_t pl = e->call(0x1020, unit, 0, 0, 0, 0, game);
    const uint32_t code = d2rd32(rec + 0x80);
    uint32_t mode;
    if (code == 0x20737361u) {            // "ass "
        e->call(0xD29F0, 0, 0, 0, 0, 0, unit);
        if (!e->call(0xA568, 0, 0, 0, 0, 0, pl, 9, 5)) { e->call(0xDFFD0, 0, 0, 0, 0, unit, 0x13, unit); return 1; }
        e->call(0xA586, 0, 0, 0, 0, 0, pl, 9, 5);
        e->call(0xA4D2, 0, 0, 0, 0, 0, unit, 5, 1, 0);
        mode = 8;
    } else if (code == 0x207A7978u) {     // "xyz "
        e->call(0xD29F0, 0, 0, 0, 0, 0, unit);
        if (!e->call(0xA568, 0, 0, 0, 0, 0, pl, 0x14, 5)) { e->call(0xDFFD0, 0, 0, 0, 0, unit, 0x13, unit); return 1; }
        e->call(0xA586, 0, 0, 0, 0, 0, pl, 0x14, 5);
        e->call(0xA4D2, 0, 0, 0, 0, 0, unit, 7, 0x1400, 0);
        mode = 0x12;
    } else if (code == 0x20327274u) {     // "tr2 "
        e->call(0xD29F0, 0, 0, 0, 0, 0, unit);
        if (!e->call(0xA568, 0, 0, 0, 0, 0, pl, 0x25, 8) || e->call(0xA568, 0, 0, 0, 0, 0, pl, 0x25, 7)) {
            e->call(0xDFFD0, 0, 0, 0, 0, unit, 0x13, unit); return 1;
        }
        e->call(0xA57A, 0, 0, 0, 0, 0, pl, 0x25, 7);
        e->call(0x5B0B0, unit, 0, 0, 0, 0, game);
        mode = 0x21;
    } else if (code == 0x20616F74u) {     // "toa "
        e->call(0xD29F0, 0, 0, 0, 0, 0, unit);
        e->call(0xA1280, unit, 0, 0, 0, 0, game);
        e->call(0xEAF80, unit);
        e->call(0xD39B0, it, unit, 0, 0, 0, game);
        e->call(0xDFFD0, 0, 0, 0, 0, unit, 2, unit);
        return 1;
    } else {
        return 0;
    }
    e->call(0x34B20, unit, 0, 0, 0, 0, mode);
    e->call(0xD39B0, it, unit, 0, 0, 0, game);
    return 1;
}
