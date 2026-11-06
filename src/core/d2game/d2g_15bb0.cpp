// 1.13c D2Game+0x15BB0: ESI = unit, stack word = game, ret 4. A monster AI tail reached through a
// function table: the list at (unit+0x14)->+0x30 is walked (+8 links); any node with a nonzero first dword returns 0. Otherwise
// 0xA496(unit, 0) and the 8-dword command {0, unit, 0, 0, 0, 0x6400, 0, 0} go to 0x75E10 (game, &block, 1) and the result is 1.
// A null unit, a non-monster or a null path makes retail read address 0; callers never pass one.
#ifndef D2G_15BB0_CPP
#define D2G_15BB0_CPP
#include "d2g_mem.h"
namespace d2g_15bb0 {
using namespace d2g;
struct Env {
    void (*a496)(P unit, I zero);              // thunk 0xA496 (stdcall 2)
    void (*cmd)(U game, U* block, I one);      // 0x75E10 (stdcall 3)
};
// 1.13c D2Game+0x15BB0
inline I Fn15BB0_113c(const Env& e, P unit, U game) {
    P list = (unit && rd(unit, 0) == 1 && rd(unit, 0x14)) ? pf(pf(unit, 0x14), 0x30) : nullptr;
    for (P n = pf(list, 0); n; n = pf(n, 8))
        if (rd(n, 0)) return 0;
    e.a496(unit, 0);
    U blk[8] = { 0, W(unit), 0, 0, 0, 0x6400, 0, 0 };
    e.cmd(game, blk, 1);
    return 1;
}
}
#endif
