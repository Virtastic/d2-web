// 1.13c D2Game+0x2A740 GAME_VerifyJoinGame: ESI = character name, 11 stack words, ret 0x2C.
// The name must end within 16 bytes, byte arg 6 be below 13; 0x498B0(arg 0, EBX = &local+4), 0x497C0(&local, EDI = name),
// 0x2A3B0(arg 1) must succeed, byte arg 2 be below 7 and 0x2A110 (EAX = name, EDI = 16) accept the name. With no ladder/realm
// callback table (DAT 0x111C3C) the join is verified; else its slot +0x18 (IsBadCodePtr-checked: a bad pointer asserts and
// exits) decides: ECX = name, EDX = arg 3, stack (word arg 1, arg 4, arg 5, arg 7, arg 8, arg 9, arg 10).
#ifndef D2G_2A740_CPP
#define D2G_2A740_CPP
#include "d2g_mem.h"
namespace d2g_2a740 {
using namespace d2g;
struct Env {
    I (*f498b0)(U* out, U a0);                 // 0x498B0: EBX = out, stack a0, ret 4
    I (*f497c0)(P name, U* local);             // 0x497C0: EDI = name, stack &local, ret 4
    I (*f2a3b0)(U id);                         // 0x2A3B0 GAME_GetGameGUIDFromGameId, ret 4
    I (*f2a110)(P name, U len);                // 0x2A110: EAX = name, EDI = 16
    U (*table)();                              // DAT 0x111C3C
    I (*badCode)(U fn);                        // IsBadCodePtr
    void (*fatal)();                           // the assert (line 0x714) and exit(-1)
    I (*verify)(U table, P name, U a3, const U* s7);   // [table+0x18]: ECX, EDX, 7 stack words
};
// 1.13c D2Game+0x2A740
inline I VerifyJoinGame_113c(const Env& e, P name, const U* a) {
    const uint8_t* s = (const uint8_t*)name;
    for (int n = 0;; n++) { if (n >= 16) return 0; if (!s[n]) break; }
    if ((uint8_t)a[6] >= 0xD) return 0;
    U loc[4];
    if (!e.f498b0(&loc[1], a[0])) return 0;
    if (!e.f497c0(name, &loc[0])) return 0;
    if (!e.f2a3b0(a[1])) return 0;
    if ((uint8_t)a[2] >= 7) return 0;
    if (!e.f2a110(name, 0x10)) return 0;
    const U t = e.table();
    if (!t) return 1;
    if (e.badCode(rd((P)(uintptr_t)t, 0x18))) e.fatal();
    const U s7[7] = { (U)(uint16_t)a[1], a[4], a[5], a[7], a[8], a[9], a[10] };
    return e.verify(t, name, a[3], s7) ? 1 : 0;
}
}
#endif
