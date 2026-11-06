// 1.13c D2Game+0x366C0: act-3 hell-portal transition. EAX = game, ESI = player, stack level,
// (unused); ret 8. Quest flags = player data (thunk 0xA4C0) +0x10 + 4 * difficulty (game +0x6D). A level other than 102
// (0x66) hands off to 0xE5A60 (EDI = game). On level 102: quest 0x17 already complete (thunk 0xA568) ends it; when quest 0x12
// is not complete it is set (thunk 0xA57A 0 then 0xD) and the five Khalim items ('qey ', 'qhr ', 'qbr ', 'qf1 ', 'qf2 ', each
// found by 0xE660 EAX = player) are deleted by 0x365E0 (EAX item, ECX player, stack game, inventory +0x60). Quest 0x17 is set
// (0, 0xD), 0x35C20 (EAX player, EDX game, CL 2) and 0x34C00 (EAX player, stack game) run, and when player data +0x4C is not
// yet 1 it is set and the 2-byte packet {0x61, 4} goes to the player's client (0x8A3E0 EAX = client, stack packet, 2).
#ifndef D2G_366C0_CPP
#define D2G_366C0_CPP
#include "d2g_mem.h"
namespace d2g_366c0 {
using namespace d2g;
struct Env {
    P (*playerData)(P unit);                       // thunk 0xA4C0 (stdcall 1)
    I (*isSet)(U q, I quest, I flag);              // thunk 0xA568 (stdcall 3)
    void (*set)(U q, I quest, I flag);             // thunk 0xA57A (stdcall 3)
    U (*findItem)(P player, U code);               // 0xE660: EAX = player, stack code, ret 4
    void (*deleteItem)(U item, P player, P game, U inv); // 0x365E0: EAX item, ECX player, stack game, inventory
    void (*text)(P player, P game, U stage);       // 0x35C20: EAX player, EDX game, CL stage
    void (*flags)(P player, P game);               // 0x34C00: EAX player, stack game, ret 4
    U (*send)(U client, U* pkt, I len);            // 0x8A3E0: EAX client, stack packet, length
    U (*other)(P game, P player);                  // 0xE5A60: EDI game (ESI player)
};
// 1.13c D2Game+0x366C0
// EAX on return: 0xE5A60's result off level 102, the quest-0x17 test when it is set, the send result when the packet goes,
// else the client word
inline U HellPortal_113c(const Env& e, P game, P player, U level) {
    P pd = e.playerData(player);
    const U q = rd(e.playerData(player), 0x10 + 4 * rd8(game, 0x6D));
    if (level != 0x66) return e.other(game, player);
    if (const I s = e.isSet(q, 0x17, 0)) return (U)s;
    if (!e.isSet(q, 0x12, 0)) {
        e.set(q, 0x12, 0);
        e.set(q, 0x12, 0xD);
        static const U codes[5] = { 0x20796571, 0x20726871, 0x20726271, 0x20316671, 0x20326671 };
        for (U c : codes) if (U it = e.findItem(player, c)) e.deleteItem(it, player, game, rd(player, 0x60));
    }
    e.set(q, 0x17, 0);
    e.set(q, 0x17, 0xD);
    e.text(player, game, 2);
    e.flags(player, game);
    const U client = (player && rd(player, 0) == 0) ? rd(e.playerData(player), 0x9C) : 0;
    if (rd(pd, 0x4C) == 1) return client;
    wr(pd, 0x4C, 1);
    U pkt = (level & 0xFFFF0000u) | 0x0461u;
    return e.send(client, &pkt, 2);
}
}
#endif
