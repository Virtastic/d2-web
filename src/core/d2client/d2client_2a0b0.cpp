// 1.13c D2Client+0x2A0B0: unit path refresh (EAX unit): when D2Common #10392 holds for the unit's path, fetch the two
// path words (#11163), run A4E20 on the per-row table entry, and clear (#10336) when #10392 changed across it.
#include "d2client_callee.h"
// 1.13c D2Client+0x2A0B0
extern "C" void d2c_2a0b0(const D2cSvc& v, const uint8_t* unit /*EAX*/) {
    if (!unit) return;                                   // retail asserts (Fog #10024, exit)
    const uint32_t path = *(const uint32_t*)(unit + 0x2c);
    if (!path) return;                                   // retail asserts
    if (!d2c_call(v, 0xc2da, 0, 0, 0, 0, 0, path)) return;                                  // D2Common #10392
    uint32_t loc[2] = {};                                                                   // loc[0] row, loc[1] column
    d2c_call(v, 0xcd54, 0, 0, 0, 0, 0, path, (uint32_t)(uintptr_t)&loc[0], (uint32_t)(uintptr_t)&loc[1]);   // D2Common #11163
    const uint32_t a = loc[0], b = loc[1];
    const uint32_t r = d2c_call(v, 0xa4e20, b & 0x7f, 0, 0x10a608u + (a << 9), 0, 0, a);
    if (r != d2c_call(v, 0xc2da, 0, 0, 0, 0, 0, path)) d2c_call(v, 0xc364, 0, 0, 0, 0, 0, path, 0);   // D2Common #10336
}
