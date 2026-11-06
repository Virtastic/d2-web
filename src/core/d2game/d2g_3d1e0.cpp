// 1.13c D2Game+0x3D1E0: periodic scatter-missile spawn. EAX = period, EBX = unit, stack game,
// range, missile id, collision mask; ret 0x10. Period 0, thunk 0xB28E (frames) % period != 0 (signed), a null unit or a clear
// bit 10 at +0xC8 give 0. The owner 0xE03A0 (ECX game, EDX +0x98, EAX +0x94) and the room thunk 0xA478 must be nonzero.
// The seed at unit+0x20 is reset (thunk 0xA628, EDX = frames + 0x1100 X); a zeroed 0x5C block gets +0 = 3, +4 = owner,
// +0x2C = thunk 0xB26A, +0x30 = thunk 0xB0FC, +0x10 = missile id, +0x14/+0x18 = X/Y (0x1100/0x1130) plus a seed roll
// 0x11D0 (ESI = 2*(range-1)) minus range-1. A collision (thunk 0xAFE8(room, x, y, mask), 16 bits) gives 0, else the result
// of 0x6F930 (ECX game, EDX block).
#ifndef D2G_3D1E0_CPP
#define D2G_3D1E0_CPP
#include "d2g_mem.h"
namespace d2g_3d1e0 {
using namespace d2g;
struct Env {
    I (*frames)(P unit);                         // thunk 0xB28E (stdcall 1)
    U (*owner)(U game, U type, U guid);          // 0xE03A0: ECX game, EAX +0x94, EDX +0x98
    U (*room)(P unit);                           // thunk 0xA478 (stdcall 1)
    I (*x)(P unit);                              // 0x1100: ECX = unit
    I (*y)(P unit);                              // 0x1130: ECX = unit
    void (*reseed)(P seed, U v);                 // thunk 0xA628: ECX seed, EDX value
    U (*skill)(P unit);                          // thunk 0xB26A (stdcall 1)
    U (*level)(P unit);                          // thunk 0xB0FC (stdcall 1)
    I (*roll)(P seed, I range);                  // 0x11D0: ECX seed, ESI range
    U (*coll)(U room, I x, I y, U mask);         // thunk 0xAFE8 (stdcall 4), low word tested
    U (*spawn)(U game, U* block);                // 0x6F930: ECX game, EDX block
};
// 1.13c D2Game+0x3D1E0
inline U PeriodicScatter_113c(const Env& e, I period, P unit, U game, I range, U id, U mask) {
    if (!period) return 0;
    if (e.frames(unit) % period) return 0;
    if (!unit || !((rd(unit, 0xC8) >> 10) & 1)) return 0;
    const U own = e.owner(game, rd(unit, 0x94), rd(unit, 0x98));
    if (!own || !e.room(unit)) return 0;
    P seed = at(unit, 0x20);
    const I f = e.frames(unit);
    e.reseed(seed, (U)(f + e.x(unit)));
    U blk[0x17] = {};
    blk[0] = 3; blk[1] = own;
    blk[0x2C / 4] = e.skill(unit);
    blk[0x30 / 4] = e.level(unit);
    const I r = range - 1;
    blk[0x10 / 4] = id;
    const I dx = e.roll(seed, 2 * r) - r;
    const I dy = e.roll(seed, 2 * r) - r;
    const I x = e.x(unit) + dx;
    blk[0x14 / 4] = (U)x;
    const I y = e.y(unit) + dy;
    blk[0x18 / 4] = (U)y;
    if ((uint16_t)e.coll(e.room(unit), x, y, mask)) return 0;
    return e.spawn(game, blk);
}
}
#endif
