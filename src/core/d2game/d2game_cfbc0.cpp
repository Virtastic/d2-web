// D2Game+0xCFBC0 -- equip an item on a mercenary: stdcall (game, player, merc, item), ret 0x10. Returns 3 without the expansion flag (game+0x70),
// 0 on a missing inventory or a refused swap, 1 once the item is on the merc. Written from the 1.13c disassembly; callees are the D2Seam.
// A868 gives the item's two body slots through out-pointers (a byte local and the merc argument slot); A7AE fetches what each slot holds.
// Merc class 0x167 with an item of type 2 (A430) takes the second slot. An occupied slot must be the merc inventory's own item (A748) and is pulled
// out (A742, D46B0, D51A0); when A934 refuses the item the old one is put back (A9A0, A9B8, A86E, A4A2, A92E, D47C0) and 0 is returned.
// Otherwise the item is copied (107E0, EAX = game), set to cursor mode 4, the merc's type-9 events are dropped (8E6F0, EAX = event, EDI = game),
// D60D0 moves the copy into the slot, the player's cursor is cleared (AA54) or given a copy of the old item (D1650), the merc is refreshed,
// its type-3 events dropped and a type-3 event set for the next frame (8EA80).
#include "d2game_seam4e.h"
// 1.13c D2Game+0xCFBC0
extern "C" uint32_t d2game_cfbc0(uint32_t game, uint32_t player, uint32_t merc, uint32_t item, const D2Seam* e)
{
    if (!d2rd32(game + 0x70)) return 3;
    const uint32_t pinv = d2rd32(player + 0x60);
    if (!pinv) return 0;
    if (!d2rd32(merc + 0x60)) e->call(0xA448, 0, 0, 0, 0, 0, 0, merc);
    const uint32_t inv = d2rd32(merc + 0x60);
    if (!inv) return 0;
    uint8_t l1[4] = { 0, 0, 0, 0 };
    uint32_t l2 = merc;   // the merc argument slot doubles as the second out-pointer
    e->call(0xA868, 0, 0, 0, 0, 0, item, (uint32_t)(uintptr_t)l1, (uint32_t)(uintptr_t)&l2);
    uint32_t old = e->call(0xA7AE, 0, 0, 0, 0, 0, inv, l1[0]);
    const uint32_t old2 = e->call(0xA7AE, 0, 0, 0, 0, 0, inv, l2 & 0xFF);
    uint32_t loc;
    if (d2rd32(merc + 4) == 0x167 && e->call(0xA430, 0, 0, 0, 0, 0, item, 2)) { old = old2; loc = l2 & 0xFF; }
    else loc = l1[0];
    if (old) {
        if (e->call(0xA748, 0, 0, 0, 0, 0, inv, old) != old) return 0;
        e->call(0xA742, 0, 0, 0, 0, 0, inv, loc);
        e->call(0xD46B0, 0, 0, 0, 0, merc, game, 0, 0); e->edi(0xD46B0, old);
        e->call(0xD51A0, 0, 0, 0, 0, 0, game, merc, 0);
        if (!e->call(0xA934, 0, 0, 0, 0, 0, item, merc, 0, 0, 0, 0)) {
            e->call(0xA9A0, 0, 0, 0, 0, 0, inv, old, loc);
            e->call(0xA9B8, 0, 0, 0, 0, 0, inv, old, 3);
            e->call(0xA86E, 0, 0, 0, 0, 0, old, loc);
            e->call(0xA4A2, 0, 0, 0, 0, 0, old, 1);
            e->call(0xA92E, 0, 0, 0, 0, 0, old, 0xFF);
            e->call(0xD47C0, 0, 0, 0, 0, merc, 0); e->edi(0xD47C0, old);
            return 0;
        }
        e->call(0xA916, 0, 0, 0, 0, 0, old, 0x10, 1);
        e->call(0xA994, 0, 0, 0, 0, 0, inv, old);
        e->call(0xA65E, 0, 0, 0, 0, 0, old, 0x20, 1);
        e->call(0xA9A6, 0, 0, 0, 0, 0, merc, 1);
    }
    uint32_t dup = e->call(0x107E0, game, 0, 0, 0, 0, item, 1);
    e->call(0xA4A2, 0, 0, 0, 0, 0, dup, 4);
    for (uint32_t ev = d2rd32(merc + 0xdc); ev;) {
        const uint32_t ty = d2rd8(ev), nx = d2rd32(ev + 0x24);
        if (ty == 9) { e->call(0x8E6F0, ev); e->edi(0x8E6F0, game); }
        ev = nx;
    }
    e->call(0xD60D0, dup ? d2rd32(dup + 0xc) : 0xFFFFFFFFu, merc, 0, 0, 0, game, loc, 1, (uint32_t)(uintptr_t)&dup);
    e->call(0xD2C70, player, 0, 0, 0, item, game);
    e->call(0xAA54, 0, 0, 0, 0, 0, pinv, 0);
    if (old) {
        const uint32_t d2 = e->call(0x107E0, game, 0, 0, 0, 0, old, 1);
        e->call(0xAA54, 0, 0, 0, 0, 0, pinv, d2);
        e->call(0xD1650, 0, 0, 0, 0, d2); e->edi(0xD1650, player);
    }
    e->call(0xD51A0, 0, 0, 0, 0, 0, game, merc, 0);
    for (uint32_t ev = d2rd32(merc + 0xdc); ev;) {
        const uint32_t ty = d2rd8(ev), nx = d2rd32(ev + 0x24);
        if (ty == 3) { e->call(0x8E6F0, ev); e->edi(0x8E6F0, game); }
        ev = nx;
    }
    e->call(0x8EA80, 0, game, merc, 0, 0, 3, d2rd32(game + 0xa8) + 1, 0, 0, 0);
    return 1;
}
