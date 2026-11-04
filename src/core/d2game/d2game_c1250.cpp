// D2Game+0xC1250 -- create a mercenary unit for a player: EBX = game, stack = player, name word, seed, version (short; -1 picks the record by act and
// difficulty), class id (0: the hireling record's), dead flag (ret 0x18). Returns the new unit or 0. Written from the 1.13c disassembly; callees are the D2Seam.
// ABF8 gives the hireling act for the name; without the expansion flag (game+0x70) the player's act (player+0x18, or the client's +0x1ac when the
// player has one) must equal it. A3BE / AC52 fetch the hireling record (never null in the captured corpus: the original asserts and exits). The name is
// clamped into the record's [first, last] range; the seller NPC's merc slot (8F5C0 -> +0x10, 16-byte entries) is marked hired when not already present.
// The class must be a monstats row whose byte +0xf shares a bit with gdwBitMasks[1]; then A6B8 allocates the unit, the fields are set inline, the
// unit id comes from game+0x94 (wraps past -1 to 1), and the unit is registered (B0320, E0F40), flagged (A688/13B0 path) and handed to 48FC0 with a
// stack block {word name, residue, dword seed}. dtCell = &sgptDataTables, bm = gdwBitMasks.
#include "d2game_seam4e.h"
// 1.13c D2Game+0xC1250
extern "C" uint32_t d2game_c1250(uint32_t game, uint32_t player, uint32_t name, uint32_t seed, uint32_t ver, uint32_t cls, uint32_t dead,
                                 uint32_t dtCell, uint32_t bm, const D2Seam* e)
{
    const uint32_t act = e->call(0xABF8, 0, d2rd32(game + 0x70), 0, 0, 0, name);   // EAX = name too (unlogged)
    uint32_t pact = d2rd8(player + 0x18);
    if (d2rd32(player) == 0 && d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9c) != 0) {
        const uint32_t client = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9c);
        if (!client) return 0;   // the original asserts and exits
        pact = d2rd8(client + 0x1ac);
    }
    const uint32_t exp = d2rd32(game + 0x70);
    if (!exp && ((int32_t)act < 0 || pact != act)) return 0;
    uint32_t rec;
    if ((ver & 0xFFFF) != 0xFFFF) rec = e->call(0xA3BE, 0, 0, 0, 0, 0, exp, ver & 0xFFFF, 1);
    else rec = e->call(0xAC52, 0, 0, 0, 0, 0, exp, act, d2rd8(game + 0x6d), 0);
    if (!rec) return 0;   // the original asserts and exits
    const uint32_t first = d2rd16(rec + 0x114);
    if (!cls) cls = d2rd32(rec + 8);
    uint32_t nm = name & 0xFFFF;
    if (nm < first || nm > d2rd16(rec + 0x116)) nm = first;
    uint32_t slot = 0;
    const uint32_t npc = e->call(0x8F5C0, (uint32_t)(uintptr_t)&slot, game, 0, 0, d2rd32(rec + 0x14));
    const uint32_t md = d2rd32(npc + 0x10);
    if (md) {
        const uint32_t m = md + ((nm - first) << 4);
        if (!d2rd32(m + 0xc)) d2wr32(m + 8, 1);
    }
    const uint32_t mode = dead ? 12 : 1;
    if ((int32_t)cls < 0) return 0;
    const uint32_t dt = d2rd32(dtCell);
    if ((int32_t)cls >= (int32_t)d2rd32(dt + 0xa80)) return 0;
    const uint32_t row = cls * 0x1a8 + d2rd32(dt + 0xa78);
    if (!row || !(d2rd8(bm + 4) & d2rd8(row + 0xf))) return 0;
    const uint32_t u = e->call(0xA6B8, 0, 0, 0, 0, 0, d2rd32(game + 0x1c), 1);
    if (!u) return 0;
    d2wr32(u + 4, cls); d2wr32(u, 1); d2wr32(u + 8, d2rd32(game + 0x1c)); d2wr32(u + 0x80, game);
    uint32_t f = d2rd32(u + 0xc8);
    f = d2rd32(game + 0x70) ? (f | 0x2000000u) : (f & 0xFDFFFFFFu);
    d2wr32(u + 0xc8, f | 0x4000000u);
    const uint32_t b = e->call(0xA56E, 0, 0, 0, 0, 0, e->call(0xA556, 0, 0, 0, 0, 0, 0)) & 0xFF;
    d2wr8(u + 0x18, b);
    d2wr32(u + 0x1c, d2rd32(game + b * 4 + 0xbc));
    d2wr32(u + 0x74, 0);
    e->call(0xE0110, 0, 0, 0, 0, game + 0xd0);
    e->edi(0xE0110, u);
    d2wr32(u + 0xc4, d2rd32(u + 0xc4) | 0x10);
    d2wr32(u + 0xd0, 0xb);
    uint32_t id = d2rd32(game + 0x94) + 1;
    if (id == 0xFFFFFFFFu) id = 1;
    d2wr32(game + 0x94, id);
    d2wr32(u + 0x10, mode);
    e->call(0xB0320, 0, game, 0, 0, 0, u, id);
    e->call(0xE0F40, 0, 0, u, 0, 0, 0, game, 1);
    if (e->call(0xA688, 0, 0, 0, 0, 0, u) && !e->call(0x13B0, cls, 0, 0x13)) {
        e->call(0xA6AC, 0, 0, 0, 0, 0, u, 1);
        e->call(0xA6D6, 0, 0, 0, 0, 0, u, 5);
        e->call(0xA682, 0, 0, 0, 0, 0, d2rd32(u + 0x2c), 0x8000);
    }
    uint32_t blk[2] = { nm, seed };
    e->call(0x48FC0, ver, player, 0, 0, 0, game, player, u, ver, (uint32_t)(uintptr_t)blk, dead);
    return u;
}
