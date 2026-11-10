// 1.13c D2Game+0xD4360: EAX = EDI = player unit, ECX = EBX = game, ESI = item; plain ret, result in EAX. A null game is fatal (0x660,
// not modelled). Thunk 0xA430(item, 0x16) set: 0xD3C40 (EAX = ESI, EBX = game, EDI = unit). Else 0xA430(item, 0x12) set: 0xD3A10(game,
// unit, item). Else thunks 0xA940(item) and 0xAA48(item) both set: 0xD3CB0(game, unit, item). Otherwise 0.
#ifndef D2G_D4360_CPP
#define D2G_D4360_CPP
#include "d2g_mem.h"
namespace d2g_d4360 {
using namespace d2g;
struct Env {
    U (*a430)(U item, I k);                       // thunk 0xA430 (2)
    U (*a940)(U item);                            // thunk 0xA940 (1)
    U (*aa48)(U item);                            // thunk 0xAA48 (1)
    U (*fd3c40)(U item, U game, U unit);          // 0xD3C40: EAX = item, EBX = game, EDI = unit
    U (*fd3a10)(U game, U unit, U item);          // 0xD3A10: 3 stack words
    U (*fd3cb0)(U game, U unit, U item);          // 0xD3CB0: 3 stack words
};
inline U Merge_113c(const Env& e, U unit, U game, U item) {
    if (e.a430(item, 0x16)) return e.fd3c40(item, game, unit);
    if (e.a430(item, 0x12)) return e.fd3a10(game, unit, item);
    if (e.a940(item) && e.aa48(item)) return e.fd3cb0(game, unit, item);
    return 0;
}
}
#endif
