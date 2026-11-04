// D2Game+0xBDB10 -- the Cain identify-all handler: ECX = player, EAX = npc guid, stack = game (ret 4). Written from the 1.13c disassembly; callees are the
// D2Seam. E03A0 (when player+0x6c is set) names the unit the player is talking to; the game's node table at +0x1320 (by guid & 0x7f) must hold a node of that
// guid that IS that unit, else an inline 15-byte packet 2A,00,09 is sent. Classes 0xF4-0xF6, 0x109 and 0x208 only; no unidentified items: reply code 9; the
// price (count * 100) is taken from gold or stash unless the quest check passes, short: reply code 0xC; then every item of the inventory list that the
// class test marks is identified through D6BC0 and one code-3 reply follows. 8AC90 is the 0x2A reply sender (AL = 0x2A, CL = code, EDX = the 10973 result,
// stack: client, -1, 0). The game == 0 abort of the original exits the process and is not reproduced.
#include "d2game_seam4e.h"
static uint32_t clientOf(uint32_t player, const D2Seam* e)
{
    if (d2rd32(player) != 0) return 0;
    return d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9c);
}
static void reply(uint32_t player, uint32_t code, const D2Seam* e)
{
    const uint32_t client = clientOf(player, e);
    const uint32_t v = e->call(0xA42A, 0, 0, 0, 0, 0, player, 0xE, 0);
    e->call(0x8AC90, 0x2A, code, v, 0, 0, client, 0xFFFFFFFFu, 0);
}
// 1.13c D2Game+0xBDB10
extern "C" uint32_t d2game_bdb10(uint32_t player, uint32_t guid, uint32_t game, const D2Seam* e)
{
    uint32_t unit = 0;
    if (d2rd8(player + 0x6c)) unit = e->call(0xE03A0, d2rd32(player + 0x68), game, d2rd32(player + 0x64));
    uint32_t node = d2rd32(game + 0x1320 + (guid & 0x7f) * 4);
    while (node && d2rd32(node + 0xC) != guid) node = d2rd32(node + 0xE4);
    if (!node || node != unit) {
        const uint32_t client = clientOf(player, e);
        const uint32_t v = e->call(0xA42A, 0, 0, 0, 0, 0, player, 0xE, 0);
        uint8_t b[15] = { 0x2A, 0, 9 };
        const uint32_t m1 = 0xFFFFFFFFu;
        memcpy(b + 7, &m1, 4); memcpy(b + 11, &v, 4);
        e->call(0x8A3E0, client, 0, 0, 0, 0, (uint32_t)(uintptr_t)b, 0xF);
        return 0;
    }
    const uint32_t cls = d2rd32(node + 4);
    if (!(cls == 0xF4 || cls == 0xF5 || cls == 0xF6 || cls == 0x109 || cls == 0x208)) return 0;
    const uint32_t unid = e->call(0xABC2, 0, 0, 0, 0, 0, player);
    if (unid == 0) { reply(player, 9, e); return 0; }
    const uint32_t pg = e->call(0xA4C0, 0, 0, 0, 0, 0, player);
    if (e->call(0xA568, 0, 0, 0, 0, 0, d2rd32(pg + 0x10 + d2rd8(game + 0x6d) * 4), 4, 0) != 1) {
        const uint32_t x = e->call(0x1020, player, 0, 0, 0, 0, game);
        if (e->call(0xA568, 0, 0, 0, 0, 0, x, 4, 1) != 1 && e->call(0xBCC60, 0, 0, 0, 0, player, unid * 100) == 0) { reply(player, 0xC, e); return 0; }
    }
    uint32_t cur = e->call(0xA3DC, 0, 0, 0, 0, 0, d2rd32(player + 0x60));
    while (cur) {
        const uint32_t next = e->call(0xA4CC, 0, 0, 0, 0, 0, cur);
        const uint32_t ty = e->call(0xA736, 0, 0, 0, 0, 0, cur);
        const uint32_t it = e->call(0xA46C, 0, 0, 0, 0, 0, cur);
        bool ident = false;
        if (ty == 1) {
            if ((e->call(0xA6C4, 0, 0, 0, 0, 0, it) & 0xFF) == 0) ident = true;
            else if ((e->call(0xA6C4, 0, 0, 0, 0, 0, it) & 0xFF) == 3) ident = true;
        } else if (ty == 3) ident = true;
        if (ident && e->call(0xA8A4, 0, 0, 0, 0, 0, it, 0x10, 0xA9C, 0xB20FB3A4u) == 0) {
            e->call(0xD6BC0, player, 0, 0, 0, 0, game);
            e->edi(0xD6BC0, it);
        }
        cur = next;
    }
    reply(player, 3, e);
    return 0;
}
