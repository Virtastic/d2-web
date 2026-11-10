// 1.13c D2Game+0xD3C40: EAX = ESI = source item pointer (or 0), EBX = game, EDI = player unit; plain ret, result in EAX. A null game is
// fatal (0x611, not modelled). Thunk 0xAA72(unit[0x60], ESI, 0) finds the target; none gives 0. Otherwise 0xD34B0 runs with EAX = ESI[0xC]
// (-1 when ESI is null), words game, unit, target[0xC], &scratch (the local the D34B0 out flag is written to); its result is returned.
#ifndef D2G_D3C40_CPP
#define D2G_D3C40_CPP
#include "d2g_mem.h"
namespace d2g_d3c40 {
using namespace d2g;
struct Env {
    P (*aa72)(U inv, P s, I z);                                       // thunk 0xAA72 (3)
    U (*fd34b0)(U idA, P game, P unit, U idB, U* out);                // 0xD34B0: EAX, 4 words
};
inline U Merge_113c(const Env& e, P s, P game, P unit) {
    P t = e.aa72(rd(unit, 0x60), s, 0);
    if (!t) return 0;
    U scratch = 0;
    const U idA = s ? rd(s, 0xC) : 0xFFFFFFFFu;
    return e.fd34b0(idA, game, unit, rd(t, 0xC), &scratch);
}
}
#endif
