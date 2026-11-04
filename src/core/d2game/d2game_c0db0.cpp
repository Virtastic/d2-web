// D2Game+0xC0DB0 -- the NPC "resurrect mercenary" handler: EAX = npc guid, ECX = game, stack = player (ret 4). Written from the 1.13c disassembly; callees are
// the D2Seam. E03A0 (when player+0x6c is set) names the unit the player is talking to; the game's node table at +0x1320 (by guid & 0x7f) must hold a node of
// that guid that IS that unit, else an inline 15-byte packet 2A,00,09 is sent. Only NPC classes 0xFC, 0x96, 0xC6, 0x16F and 0x203 hire/revive; any other
// class gets reply code 9. 96A40 (EAX = player, EBX = 1, ESI = 7, EDI = game) finds the player's dead merc; none: the 15-byte packet. Gold is taken through
// BCC60 (ESI = player); short: reply code 0xC. Otherwise the merc is revived (flag 0x10000 cleared, A4A2/A3A0/A472), C0B90 re-links it, 0x9B goes out
// (8A690) and reply code 5 carries the merc id. 8AC90 is the 0x2A reply sender (AL = 0x2A, CL = code, EDX = the 10973 result, stack: client, id/-1, 0).
// The game == 0 abort of the original exits the process and is not reproduced.
#include "d2game_seam4e.h"
static uint32_t clientOf(uint32_t player, const D2Seam* e)
{
    if (d2rd32(player) != 0) return 0;
    return d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9c);
}
static void reply(uint32_t player, uint32_t code, uint32_t id, const D2Seam* e)
{
    const uint32_t client = clientOf(player, e);
    const uint32_t v = e->call(0xA42A, 0, 0, 0, 0, 0, player, 0xE, 0);
    e->call(0x8AC90, 0x2A, code, v, 0, 0, client, id, 0);
}
static void packet15(uint32_t player, const D2Seam* e)
{
    const uint32_t client = clientOf(player, e);
    const uint32_t v = e->call(0xA42A, 0, 0, 0, 0, 0, player, 0xE, 0);
    uint8_t b[15] = { 0x2A, 0, 9 };
    const uint32_t m1 = 0xFFFFFFFFu;
    memcpy(b + 7, &m1, 4); memcpy(b + 11, &v, 4);
    e->call(0x8A3E0, client, 0, 0, 0, 0, (uint32_t)(uintptr_t)b, 0xF);
}
// 1.13c D2Game+0xC0DB0
extern "C" void d2game_c0db0(uint32_t guid, uint32_t game, uint32_t player, const D2Seam* e)
{
    uint32_t unit = 0;
    if (d2rd8(player + 0x6c)) unit = e->call(0xE03A0, d2rd32(player + 0x68), game, d2rd32(player + 0x64));
    uint32_t node = d2rd32(game + 0x1320 + (guid & 0x7f) * 4);
    while (node && d2rd32(node + 0xC) != guid) node = d2rd32(node + 0xE4);
    if (!node || node != unit) { packet15(player, e); return; }
    const uint32_t cls = d2rd32(node + 4);
    if (!(cls == 0xFC || cls == 0x96 || cls == 0xC6 || cls == 0x16F || cls == 0x203)) { reply(player, 9, 0xFFFFFFFFu, e); return; }
    const uint32_t m = e->call(0x96A40, player, 0, 0, 1, 7);
    e->edi(0x96A40, game);
    if (!m) { packet15(player, e); return; }
    const uint32_t cost = e->call(0xAC22, 0, 0, 0, 0, 0, m);
    if (!e->call(0xBCC60, 0, 0, 0, 0, player, cost)) { reply(player, 0xC, 0xFFFFFFFFu, e); return; }
    d2wr32(m + 0xC4, d2rd32(m + 0xC4) & 0xFFFEFFFFu);
    e->call(0xA4A2, 0, 0, 0, 0, 0, m, 1);
    const uint32_t v = e->call(0xA3A0, 0, 0, 0, 0, 0, m, 0);
    e->call(0xA472, 0, 0, 0, 0, 0, m, 6, v);
    e->call(0xC0B90, m, 0, 0, 0, 0, game, player);
    e->call(0x8A690, 0xFFFF, 0, 0, 0, 0, clientOf(player, e));
    reply(player, 5, d2rd32(m + 0xC), e);
}
