// 1.13c D2Game+0x91400: stdcall (owner, unit, acc), ret 0xC. Stat 0x15E of the unit (thunk
// 0xA712, layer 0) indexes a 0x23C-byte data-table row (sgptDataTables +0xB98, count +0xBA0). The level is max(stat 0x15F,
// thunk 0xA400(owner, thunk 0xA3F4(ECX = owner, EDX = row index, stack 1))); when positive and the row's stat word +0x54 is a valid
// stat (count +0xBD4) with a positive value on the unit, the row bonus (0x1B10: (level-1) * row+0x14C + row+0x148) is added to
// acc+0x38 (value 1), acc+0x38 and +0x3C (value 2), or both twice (value 3).
#ifndef D2G_91400_CPP
#define D2G_91400_CPP
#include "d2g_mem.h"
namespace d2g_91400 {
using namespace d2g;
struct Env {
    P (*tables)();                         // *sgptDataTables
    I (*stat)(U unit, I id, I layer);      // thunk 0xA712 (stdcall 3)
    I (*a3f4)(U owner, I row, I one);      // thunk 0xA3F4: ECX, EDX, 1 stack word
    I (*a400)(U owner, I v);               // thunk 0xA400 (stdcall 2)
};
// 1.13c D2Game+0x1B10 (EAX = row index, EDX = level): the per-level row bonus
inline I RowBonus_113c(P dt, I row, I lvl) {
    if (row < 0 || row >= rds(dt, 0xBA0)) return 0;
    const U r = rd(dt, 0xB98) + (U)row * 0x23C;
    if (!r || lvl <= 0) return 0;
    return (lvl - 1) * rds((P)(uintptr_t)r, 0x14C) + rds((P)(uintptr_t)r, 0x148);
}
// 1.13c D2Game+0x91400
inline void Fn91400_113c(const Env& e, U owner, U unit, P acc) {
    P dt = e.tables();
    const I row = e.stat(unit, 0x15E, 0);
    if (row < 0 || row >= rds(dt, 0xBA0)) return;
    const U r = rd(dt, 0xB98) + (U)row * 0x23C;
    if (!r) return;
    I lvl = e.stat(unit, 0x15F, 0);
    const I m = e.a400(owner, e.a3f4(owner, row, 1));
    if (m > lvl) lvl = m;
    if (lvl <= 0) return;
    const int16_t s = (int16_t)rd16((P)(uintptr_t)r, 0x54);
    if (s < 0 || s >= rds(dt, 0xBD4)) return;
    const I k = e.stat(unit, s, 0);
    if (k <= 0) return;
    I add = RowBonus_113c(dt, row, lvl);
    if (k == 1) { wr(acc, 0x38, rd(acc, 0x38) + (U)add); return; }
    if (k == 3) add *= 2;
    else if (k != 2) return;
    wr(acc, 0x38, rd(acc, 0x38) + (U)add);
    wr(acc, 0x3C, rd(acc, 0x3C) + (U)add);
}
}
#endif
