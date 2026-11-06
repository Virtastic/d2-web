// D2Client rva 0xC5F20 (SkillsBar.cpp): set up a skill-bar overlay on the unit's +0x2C handle (EDI = self, stack p).
// Two thunk lookups on p (0xCB68 / 0xCB6E) must both succeed; the handle is then configured (0xC60A on p, 0xC3AC, 0xC400,
// 0xC3EE with both lookups, 0xC38E 9), its frame word comes from the class record (self type 1: 0x1230 +0x34 signed
// short; else 0x1300 +0x41 byte; EAX = self+4 for both) shifted left 8, and it is attached to self (0xC376). Retail order.
#include <cstdint>
#include "d2client_callee.h"

// 1.13c D2Client+0xC5F20 (EDI = self, ret 4)
extern "C" uint32_t d2_client_c5f20(const D2cSvc& v, uint32_t self, const uint32_t* selfp, uint32_t p) {
    const uint32_t h = selfp[0xb];
    if (!h) return 0;
    const uint32_t a = d2c_call(v, 0xcb68, 0, 0, 0, 0, 0, p);
    const uint32_t b = d2c_call(v, 0xcb6e, 0, 0, 0, 0, 0, p);
    if (!a || !b) return 0;
    d2c_call(v, 0xc60a, 0, 0, 0, 0, 0, p, 0x1101);
    d2c_call(v, 0xc3ac, 0, 0, 0, 0, 0, h, 0);
    d2c_call(v, 0xc400, 0, 0, 0, 0, 0, h, 0);
    d2c_call(v, 0xc3ee, 0, 0, 0, 0, 0, h, a, b);
    d2c_call(v, 0xc38e, 0, 0, 0, 0, 0, h, 9);
    uint32_t w;
    if (selfp[0] == 1) {
        const uint32_t r = d2c_call(v, 0x1230, selfp[1], 0, 0, 0, 0);
        if (!r) return 0;
        w = (uint32_t)(int32_t)*(const int16_t*)(uintptr_t)(r + 0x34) << 8;
    } else {
        const uint32_t r = d2c_call(v, 0x1300, selfp[1], 0, 0, 0, 0);
        w = (uint32_t)*(const uint8_t*)(uintptr_t)(r + 0x41) << 8;
    }
    d2c_call(v, 0xc328, 0, 0, 0, 0, 0, h, w, 0xB2000000u | 0xd5f6c, 0x237);   // SkillsBar.cpp
    d2c_call(v, 0xc376, 0, 0, 0, 0, 0, h, self, 0);
    return 1;
}
