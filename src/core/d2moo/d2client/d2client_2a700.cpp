// 1.13c D2Client+0x2A700: unit refresh: 2A0B0, then two D2Common #10494 state gates (0xD -> 74480, 0x16 -> 578F0),
// result = D2Common #10342(unit, global 0x119824) == 0.
#include "d2client_callee.h"
// 1.13c D2Client+0x2A700
extern "C" uint32_t d2c_2a700(const D2cSvc& v, uint32_t unit /*EAX*/, uint32_t g119824) {
    d2c_call(v, 0x2a0b0, unit, 0, 0, 0, 0);
    if (d2c_call(v, 0xc1fc, 0, 0, 0, 0, 0, unit, 0xd)) d2c_call(v, 0x74480, 0, 0, 0, unit, 0);   // D2Common #10494
    if (d2c_call(v, 0xc1fc, 0, 0, 0, 0, 0, unit, 0x16)) d2c_call(v, 0x578f0, 0, 0, 0, 0, 0);
    return d2c_call(v, 0xc3e2, 0, 0, 0, 0, 0, unit, g119824) == 0 ? 1u : 0u;                    // D2Common #10342
}
