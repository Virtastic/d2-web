// 1.13c D2Game+0x5C690: act-5 quest 3, spawn Drehya (monster 0x200) at her town object and open the town portal (object
// 0xBD) at her feet. stdcall (quest, questEx), ret 8. Written from the 1.13c disassembly;
// differs from D2MOO 1.10f in order: the object's room and X/Y are read before the "monster already spawned" test, and the
// object lookup (game +0x1520 object hash, chain +0xE4) is inlined. Callees go through Env so the FN probe can record them.
#ifndef D2GAME_5C690_CPP
#define D2GAME_5C690_CPP
#include "d2g_mem.h"
namespace d2game_5c690 {
using namespace d2g;
struct Env {
    void (*fatal)();                               // game NULL: Fog 10265/10024 then _exit(-1)
    P (*room)(P unit);                             // thunk 0xA478 (stdcall 1): unit's room
    I (*pathX)(P path);                            // thunk 0xA454 (stdcall 1)
    I (*pathY)(P path);                            // thunk 0xA3B8 (stdcall 1)
    P (*spawn)(P game, I x, I y, P room, I one, I cls); // 0x361E0 (stdcall 6)
    U (*mapAi)(P mon);                             // 0x20DE0: EAX = monster
    void (*changeMapAi)(P slot, U ai);             // thunk 0xA562: ECX = &questEx+0x104, EDX = map AI
    void (*coords)(P unit, U* xy);                 // thunk 0xA544 (stdcall 2)
    P (*roomAt)(P room, I x, I y);                 // 0x1280: EAX = room, EBX = x, EDI = y
    P (*alloc)(I type, I cls, I x, I y, P game, P room, I a, I b, I c); // 0xE1D90: ECX type, EDX class, stack 7
    void (*seq)(P quest);                          // 0x56B10: ECX = quest
};
static inline bool isStatic(U t) { return t == 2 || (t > 3 && t <= 5); }   // object, missile? (types 2, 4, 5): static path
// 1.13c D2Game+0x5C690
inline void SpawnDrehyaInTown_113c(const Env& e, P quest, P ex) {
    if (rd8(ex, 0xE1) != 1) return;
    P game = pf(quest, 4);
    const U guid = rd(ex, 0xE4);
    if (!game) { e.fatal(); return; }
    P u = pf(game, 0x1520 + (guid & 0x7F) * 4);
    for (; u; u = pf(u, 0xE4)) if (rd(u, 0xC) == guid) break;
    if (!u) return;
    P r = e.room(u);
    const U t = rd(u, 0);
    const I x = isStatic(t) ? rds(pf(u, 0x2C), 0xC) : (pf(u, 0x2C) ? e.pathX(pf(u, 0x2C)) : 0);
    const U t2 = rd(u, 0);
    const I y = isStatic(t2) ? rds(pf(u, 0x2C), 0x10) : (pf(u, 0x2C) ? e.pathY(pf(u, 0x2C)) : 0);
    if (rd8(ex, 0x91)) return;
    P mon = e.spawn(pf(quest, 4), x, y, r, 1, 0x200);
    if (!mon) return;
    wr(ex, 0x94, rd(mon, 0xC));
    wr8(ex, 0x91, 1);
    const U ai = e.mapAi(mon);
    P slot = pf(ex, 0x104);
    if (slot && rd(slot, 4) && !rd8(ex, 0x10C)) {
        wr8(ex, 0x10C, 1);
        e.changeMapAi(at(ex, 0x104), ai);
    }
    U xy[2] = { 0, 0 };
    e.coords(mon, xy);
    P mr = e.room(mon);
    P pr = e.roomAt(mr, (I)xy[0], (I)xy[1]);
    if (pr) {
        if (P portal = e.alloc(2, 0xBD, (I)xy[0], (I)xy[1], pf(quest, 4), pr, 1, 1, 0)) {
            wr8(ex, 0xB2, 1);
            wr(ex, 0xB4, rd(portal, 0xC));
        }
    }
    e.seq(quest);
}
}
#endif
