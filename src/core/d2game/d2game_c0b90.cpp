// D2Game+0xC0B90 -- the summon/NPC link teardown: EAX = unit, stack = game, rec (ret 8); nothing is returned. With the record's per-game data
// (A4C0) linked to a list entry whose flag bit 0 is clear (only while the data-table word at +0xBF0 is above 7) the entry's key is looked up in the game's
// node table (+0x1320, by key & 0x7f); a type-1 node resets its unit (+0x2C = -1, +0x30 = 1, +0x28 = game) and is handed to B7B50 / A478 / A44E / E20A0.
// Either way the unit is then stripped (A4A2, A3A0, A472, B6D70, EFE40, three A3CA, the flag words +0xC4 |= 0x402000E, +0xC8 &= 0xFFFBFFFF, BCA20) and the
// record's pointer is passed on (B7340, 48950). Record types 2 and 4..5 read it as a struct (+0x10, +0xC); the others go through A3B8 / A454.
// dpp is the cell the module global 0x6fd1829c points at (it holds the data-table pointer). The game == 0 abort of the original exits the process.
#include "d2game_seam4e.h"
extern "C" void d2game_c0b90(uint32_t unit, uint32_t game, uint32_t rec, uint32_t dpp, const D2Seam* e)
{
    if (!unit || !rec) return;
    const uint32_t pg = e->call(0xA4C0, 0, 0, 0, 0, 0, rec);
    if (pg && (int32_t)d2rd32(d2rd32(dpp) + 0xBF0) > 7 && d2rd32(pg + 0x44)) {
        uint32_t ent = d2rd32(d2rd32(d2rd32(pg + 0x44)) + 0x54);
        while (ent && (d2rd8(ent) & 1)) ent = d2rd32(ent + 0x14);
        if (ent) {
            const uint32_t key = d2rd32(ent + 4);
            uint32_t node = d2rd32(game + 0x1320 + (key & 0x7f) * 4);
            while (node && d2rd32(node + 0xC) != key) node = d2rd32(node + 0xE4);
            if (node) {
                if (d2rd32(node) == 1) {
                    const uint32_t nb = d2rd32(node + 0x14);
                    const uint32_t u = nb ? d2rd32(nb + 0x28) : 0;
                    d2wr32(u + 0x2C, 0xFFFFFFFFu); d2wr32(u + 0x30, 1); d2wr32(u + 0x28, game);
                }
                e->call(0x97B50, 0, game, rec, 0, 0, d2rd32(node + 0xC), 0);
                const uint32_t r = e->call(0xA478, 0, 0, 0, 0, 0, node, 1, d2rd32(node + 0xC));
                e->call(0xA44E, 0, 0, 0, 0, 0, r);
                e->call(0xE20A0, 0, game, node, 0, 0);
            }
        }
    }
    e->call(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
    const uint32_t v = e->call(0xA3A0, 0, 0, 0, 0, 0, unit, 0);
    e->call(0xA472, 0, 0, 0, 0, 0, unit, 6, v);
    e->call(0x96D70, unit, 0, 0, 0, 0, game, rec);
    e->call(0xEFE40, 0, game, 0, 0, 0, 0);
    e->call(0xA3CA, 0, 0, 0, 0, 0, unit, 1, 0);
    e->call(0xA3CA, 0, 0, 0, 0, 0, unit, 0x6B, 0);
    e->call(0xA3CA, 0, 0, 0, 0, 0, unit, 0x9A, 0);
    d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x402000Eu);
    d2wr32(unit + 0xC8, d2rd32(unit + 0xC8) & 0xFFFBFFFFu);
    e->call(0xBCA20, 0, 0, 0, unit, 0);
    const int32_t ty = (int32_t)d2rd32(rec);
    const bool st = ty == 2 || (ty > 3 && ty <= 5);
    const uint32_t p = d2rd32(rec + 0x2C);
    uint32_t arg1 = 0, arg2 = 0;
    if (st) { arg1 = d2rd32(d2rd32(rec + 0x2C) + 0x10); arg2 = d2rd32(d2rd32(rec + 0x2C) + 0xC); }
    else {
        if (p) arg1 = e->call(0xA3B8, 0, 0, 0, 0, 0, p);
        if (p) arg2 = e->call(0xA454, 0, 0, 0, 0, 0, p);
    }
    e->call(0x97340, game, 0, 0, 0, 0, rec, arg2, arg1);
    e->call(0x48950, rec, game, 0, 0, 0);
}
