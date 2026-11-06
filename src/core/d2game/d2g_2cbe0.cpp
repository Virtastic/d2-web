// 1.13c D2Game+0x2CBE0: EAX = game, no stack args. A null game is fatal (line 0xC0E).
// For each of the five acts at game+0xBC with a pending flag (+0x54): walk the act's rooms (thunk 0xA5C8 first, +0x7C
// next); step each room's 64-bit seed at +0x6C (lo * 0x6AC690C5 + hi); when the new low word has no bits in 0x7FFF and
// +0x78 is clear, thunk 0xA556 gives a level id and thunk 0xA74E its record, and a record with byte +0x2E set calls 0x47820
// (ECX = room, stack id, record). A room without flag bit 0 at +0x34 is initialised (0xE1430 EAX = room ECX = game, then
// 0x200E0, 0x2F0D0, 0x47A20 on (game, room)), flagged, and thunk 0xB300(room, 1); a flagged room that thunk 0xB318 reports
// idle gets 0x200E0 and 0xB300(room, 1). The act's pending flag is then cleared.
#ifndef D2G_2CBE0_CPP
#define D2G_2CBE0_CPP
#include "d2g_mem.h"
namespace d2g_2cbe0 {
using namespace d2g;
struct Env {
    void (*fatal)(I line);                 // thunks 0xA2DA, 0xA2C2, 0x2A3C(-1)
    P (*firstRoom)(P act);                 // thunk 0xA5C8 (stdcall 1)
    U (*levelId)(P room);                  // thunk 0xA556 (stdcall 1)
    P (*levelRec)(U id);                   // thunk 0xA74E (stdcall 1)
    void (*f47820)(P room, U id, P rec);   // 0x47820: ECX = room, stack id, record
    void (*fe1430)(P room, P game);        // 0xE1430: EAX = room, ECX = game
    void (*f200e0)(P game, P room);        // 0x200E0 (stdcall 2)
    void (*f2f0d0)(P game, P room);        // 0x2F0D0 (stdcall 2)
    void (*f47a20)(P game, P room);        // 0x47A20 (stdcall 2)
    I (*idle)(P room);                     // thunk 0xB318 (stdcall 1)
    void (*touch)(P room, I one);          // thunk 0xB300 (stdcall 2)
};
// 1.13c D2Game+0x2CBE0
inline void ActRoomSweep_113c(const Env& e, P game) {
    if (!game) { e.fatal(0xC0E); return; }
    for (U k = 0; k < 5; ++k) {
        P act = pf(game, 0xBC + 4 * k);
        if (!act || !rd(act, 0x54)) continue;
        for (P room = e.firstRoom(act); room; room = pf(room, 0x7C)) {
            const uint64_t v = (uint64_t)rd(room, 0x6C) * 0x6AC690C5ull + rd(room, 0x70);
            wr(room, 0x6C, (U)v); wr(room, 0x70, (U)(v >> 32));
            if (!((U)v & 0x7FFF) && !rd(room, 0x78)) {
                const U id = e.levelId(room);
                P rec = e.levelRec(id);
                if (rd8(rec, 0x2E) && W(rec) + 0x1C != 0) e.f47820(room, id, rec);
            }
            if (!(rd8(room, 0x34) & 1)) {
                e.fe1430(room, game);
                e.f200e0(game, room);
                e.f2f0d0(game, room);
                e.f47a20(game, room);
                wr(room, 0x34, rd(room, 0x34) | 1);
            } else if (e.idle(room)) {
                continue;
            } else {
                e.f200e0(game, room);
            }
            e.touch(room, 1);
        }
        wr(act, 0x54, 0);
    }
}
}
#endif
