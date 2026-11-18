// 1.13c D2Client+0x2A6C0: clear unit flag 0x40 at +0xC4, then when D2Common #11090 accepts (unit, arg) run the three follow-ups.
#include "d2client_callee.h"
// 1.13c D2Client+0x2A6C0
extern "C" void d2c_2a6c0(const D2cSvc& v, uint8_t* unit /*ESI*/, uint32_t arg /*EDI*/) {
    const uint32_t u = (uint32_t)(uintptr_t)unit;
    if (unit) *(uint32_t*)(unit + 0xc4) &= 0xffffffbfu;
    if (!d2c_call(v, 0xc43c, 0, 0, 0, 0, 0, u, arg)) return;   // D2Common #11090
    d2c_call(v, 0x6a220, u, 0, 0, 0, 0, 0);
    d2c_call(v, 0x2a590, 0, arg, u, 0, 0);
    d2c_call(v, 0x24490, u, 0, 0, 0, 0, arg, 0);
}
