// D2Client rva 0xA5A60: check that a position (ECX = x, EAX = y) lies in a room. With a control unit (D2Client+0x11BBFC)
// its room (thunk 0xC1F0) is tried first through 0x1400 (EAX = room, EBX = x, EDI = y); otherwise or on a miss the act
// (D2Client+0x11C3B8) is asked through thunk 0xC2E0 (act, x, y). Neither: the assert text is formatted (0xBEA2, cdecl),
// logged (0xBF92 / 0xBF80) and the process exits (0x3664). Returns the hit (EAX as retail leaves it).
#include <cstdint>
#include "d2client_callee.h"

// 1.13c D2Client+0xA5A60
extern "C" uint32_t d2_client_a5a60(const D2cSvc& v, uint32_t x, uint32_t y, uint32_t ctl, uint32_t act) {
    uint32_t room = 0;
    if (x == 0 && y == 0) return y;
    if (ctl) {
        room = d2c_call(v, 0xc1f0, 0, 0, 0, 0, 0, ctl);
        if (const uint32_t r = d2c_call(v, 0x1400, room, 0, 0, x, 0)) return r;
    }
    if (const uint32_t r = d2c_call(v, 0xc2e0, 0, 0, 0, 0, 0, act, x, y)) return r;
    char buf[0x200];
    d2c_call(v, 0xbea2, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)buf, 0x200, 0xB2000000u | 0xd4168, ctl, room, act);   // + x, y
    const uint32_t m = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, 0x13c);
    d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xB2000000u | 0xd359f, m);
    d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
    return 0;
}
