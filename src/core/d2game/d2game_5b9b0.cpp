// 1.13c D2Game+0x5B9B0: act-5 quest (node type 0x20) per-player step: find the class-0x1B2 monster (mode 0xC) near the
// player in the rooms around `unit`, drop object 0xBD at its feet and run the quest-state tail. stdcall (game, unit,
// player), ret 0xC. Written from the 1.13c disassembly; not in D2MOO 1.10f.
// Callees go through Env so the FN probe can record them in retail's call order.
#ifndef D2GAME_5B9B0_CPP
#define D2GAME_5B9B0_CPP
#include "d2g_mem.h"
namespace d2game_5b9b0 {
using namespace d2g;
struct Env {
    I (*slot)(P node, P player);                   // 0x58260: ECX = node, EAX = player; player slot index
    P (*room)(P unit);                             // thunk 0xA478 (stdcall 1)
    void (*nearRooms)(P room, U* rooms, U* count); // thunk 0xA55C (stdcall 3): rooms = pointer to the room array
    I (*dist)(P unit, P other);                    // thunk 0xAB86 (stdcall 2)
    void (*coords)(P unit, I* xy);                 // thunk 0xA544 (stdcall 2): writes X, Y
    P (*roomAt)(P room, I x, I y);                 // 0x1280: EAX = room, EBX = x, EDI = y
    P (*alloc)(I type, I cls, I x, I y, P game, P room, I a, I b, I c); // 0xE1D90: ECX type, EDX class, stack 7
    void (*freeSpot)(P room, I* xy, I two, I flags, U* out, I tries);   // 0x34130 (stdcall 6)
    void (*events)(P game, P unit, I seven, I frame, I a, I b, I c);    // 0x8EA80: ECX game, EDX unit, stack 5
    void (*roomState)(P room, I z);                // thunk 0xA51A (stdcall 2)
    P (*client)(P player);                         // thunk 0xA4C0 (stdcall 1)
    U (*qget)(P q, I quest, I flag);               // thunk 0xA568 (stdcall 3)
    void (*qset)(P q, I quest, I flag);            // thunk 0xA57A (stdcall 3)
    void (*dispatch)(P game, U cb, P arg);         // 0xDFCC0: EAX = 0, EBX = callback, EDI = game, stack arg
};
enum : U { CB_593F0 = 0x593F0, CB_59390 = 0x59390, CB_5B180 = 0x5B180, CB_592F0 = 0x592F0 };   // callback RVAs handed to 0xDFCC0
// 1.13c D2Game+0x5B9B0
inline void Step_113c(const Env& e, P game, P unit, P player) {
    P head = pf(game, 0x10F4);
    if (!head) return;
    P node = pf(head, 0);
    for (; node; node = pf(node, 0xF4)) if (rd(node, 0) == 0x20) break;
    if (!node) return;
    P data = pf(node, 0x18);
    const I idx = e.slot(node, player);
    if (idx < 0) return;
    P flag = at(data, (U)idx + 0x10E);
    if (rd8(flag, 0) == 1) return;
    I xy[2] = { (I)rd(data, 0x8C + (U)idx * 8) + 0xC, (I)rd(data, 0x90 + (U)idx * 8) };
    P room = e.room(player);
    P r2 = e.room(unit);
    if (!r2) return;
    U rooms = 0, cnt = 0;
    e.nearRooms(r2, &rooms, &cnt);
    bool found = false;
    for (U i = 0; i < cnt; ++i) {
        for (P u = pf(pf((P)(uintptr_t)rooms, i * 4), 0x74); u; u = pf(u, 0xE8)) {
            if (rd(u, 0) == 1 && rd(u, 4) == 0x1B2 && rd(u, 0x10) == 0xC && e.dist(u, player) < 0xF) {
                room = e.room(u);
                e.coords(u, xy);
                xy[0] += 2;
                found = true;
                break;                               // next room; a later match overwrites
            }
        }
    }
    if (!found) return;
    wr8(flag, 0, 1);
    P h = e.roomAt(room, xy[0], xy[1]);
    P gm = pf(node, 4);
    P s = e.alloc(2, 0xBD, xy[0], xy[1], gm, h, 1, 1, 0);
    if (!s) s = e.alloc(2, 0xBD, xy[0] - 2, xy[1], gm, h, 1, 1, 0);
    if (!s && h) {
        U out = W(h);                                 // retail reuses the player argument slot as the out cell, seeded with h
        e.freeSpot(h, xy, 2, 0x8000, &out, 0x64);
        s = e.alloc(2, 0xBD, xy[0], xy[1], gm, out ? (P)(uintptr_t)out : h, 1, 1, 0);
    }
    if (s) {
        wr8(data, 0xF8 + (U)idx, 1);
        wr(data, 0xEC + (U)idx * 4, rd(s, 0xC));
        e.events(gm, s, 7, (I)rd(gm, 0xA8) + 0x19, 0, 0, 0);
        e.roomState(e.room(s), 0);
    }
    P cd = e.client(unit);
    P q = pf(cd, 0x10 + (U)rd8(game, 0x6D) * 4);
    const I ac = (I)rd(data, 0xAC), a8 = (I)rd(data, 0xA8);
    if ((I)rd(data, 0xA4) == a8 + ac && rd8(data, 0x86) && rd8(data, 0x87) && rd8(data, 0x88)) {
        if (ac <= 0xB) { e.dispatch(game, CB_59390, unit); return; }
        wr8(node, 0x14, 0); wr8(node, 0xB, 3);
        e.dispatch(gm, CB_593F0, nullptr);
        if (!e.qget(q, 0x24, 0) && !e.qget(q, 0x24, 1)) {
            e.qset(q, 0x24, 0xD);
            e.qset(q, 0x24, 1);
            const I ac2 = (I)rd(data, 0xAC);
            e.qset(q, 0x24, ac2 == 0xF ? 5 : ac2 == 0xE ? 6 : 7);
        }
        e.dispatch(game, CB_5B180, unit);
        e.dispatch(game, CB_59390, unit);
        e.dispatch(game, CB_592F0, unit);
        return;
    }
    e.qset(q, 0x24, 4);
    if ((I)rd(data, 0xA8) < 5) {
        wr8(node, 0x14, 0x20); wr8(node, 0xB, 2);
        e.dispatch(gm, CB_593F0, nullptr);
    }
    wr8(data, 0x84, 0);
}
}
#endif
