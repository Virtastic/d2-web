// 1.13c D2Game+0xEDB50 -- read a player save: the section readers run in order over one cursor. EAX = length; stack (ret 0x1c):
// game, client, buffer, ppPlayer, three unused. Returns a code from the 27-entry table at 0xF9F68 indexed by the failing reader's status
// (negative or >= 0x1b maps to 1); on failure the client's +0x174 is cleared and a half-built player is torn down.
// Frame: L1 cursor (E-0x10), L3 reader scratch (E-0xC), L4 / L2 the stats read for the 0xECC90 finish (E-4 / E-8). The slot of the
// ppPlayer word is reused for the header dword buffer+4 once the player exists.
// Readers: 0xED250 (EAX = pEnd, ECX = game; &cursor, ppPlayer), 0xED540, 0xEB560 / 0xEB4E0 (ECX = player; &cursor, pEnd), 0xEB440,
// 0xEBDA0, 0xECA60, 0xECB50, 0xECF30, 0xED960, 0xECBD0, 0xED880, 0xECC90, 0xA42A (stat), 0xA478 / 0xA44E / 0xE20A0 (teardown).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_edb50(uint32_t len, uint32_t game, uint32_t client, uint32_t buffer, uint32_t ppPlayer,
                               uint32_t, uint32_t, uint32_t, const D2Seam* e)
{
    static const uint32_t tbl[27] = {0x0, 0xe, 0x9, 0xe, 0x9, 0xe, 0xe, 0x1, 0x18, 0x17, 0x15, 0x14, 0x13, 0x11, 0x12, 0x2, 0x3, 0xb,
                                     0x4, 0x5, 0x7, 0x8, 0xa, 0xa, 0xe, 0x1a, 0x19};
    uint32_t cur = buffer, l3 = 0;
    const uint32_t pEnd = buffer + len;
    d2wr32(ppPlayer, 0);
    if (!client) {
        const uint32_t m = e->call(0xA2DA, 0, 0, 0, 0, 0, 0x114);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9u, m, 0x114);
        e->call(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        return 0;
    }
    const uint32_t pCur = (uint32_t)(uintptr_t)&cur, pL3 = (uint32_t)(uintptr_t)&l3;
    uint32_t player = 0, header = ppPlayer, l4 = 0, l2 = 0;
    int32_t st = (int32_t)e->call(0xED250, pEnd, game, 0, 0, 0, pCur, ppPlayer);
    if (st == 0) {
        player = d2rd32(ppPlayer);
        header = d2rd32(buffer + 4);
        auto R = [&](uint32_t v) { st = (int32_t)v; return st != 0; };
        bool bad =
            R(e->call(0xEB560, 0, player, 0, 0, 0, pCur, pEnd)) ||
            R(e->call(0xEB4E0, 0, player, 0, 0, 0, pCur, pEnd)) ||
            R(e->call(0xEB440, pEnd, 0, 0, pCur, 0, player));
        if (!bad) bad = R(e->call(0xEBDA0, header, pEnd, game, 0, 0, game, player, pCur, d2rd8(buffer + 0x29)));
        if (!bad) {
            l4 = e->call(0xA42A, 0, 0, 0, 0, 0, player, 6, 0);
            l2 = e->call(0xA42A, 0, 0, 0, 0, 0, player, 8, 0);
            bad = R(e->call(0xECA60, player, 0, pEnd, 0, 0, pCur, d2rd8(buffer + 0x2a))) ||
                  R(e->call(0xECB50, pEnd, 0, header, pCur, 0, game, pL3)) ||
                  R(e->call(0xECF30, 0, game, header, 0, 0, game, player, pCur, pEnd, header, pL3));
        }
        if (!bad) {
            const uint32_t self = e->call(0xED960, buffer + 0xaf, 0, 0, 0, game, game, player);
            bad = R(e->call(0xECBD0, pEnd, self, 0, self, 0, pCur, header, pL3));
            if (!bad) {
                if (self) e->call(0x48950, player, game);
                bad = R(e->call(0xED880, game, 0, 0, pEnd, player, pCur, header));
                if (!bad) {
                    e->call(0xECC90, 0, 0, 0, 0, player, game, l4, l2);
                    return 0;
                }
            }
        }
    } else if (st == 2) {
        if (cur == pEnd) return e->call(0xED540, client, 0, 0, 0, 0, client, ppPlayer);
        d2wr32(client + 0x174, 0);
        return tbl[2];
    }
    if (st < 0 || st >= 0x1b) st = 1;
    if (player) {
        if (e->call(0xA478, 0, 0, 0, 0, 0, player)) {
            const uint32_t room = e->call(0xA478, 0, 0, 0, 0, 0, player);
            e->call(0xA44E, 0, 0, 0, 0, 0, room, d2rd32(player), d2rd32(player + 0xc));
        }
        e->call(0xE20A0, 0, game, player);
    }
    d2wr32(client + 0x174, 0);
    return tbl[st];
}
