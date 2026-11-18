// 1.13c D2Client+0x29F30: mode/skill gate. ESI unit. 1 = allowed unless the unit's skill row exists, its byte +7 misses the
// global mask byte (+0x1C) and the unit is in neither mode 1 nor 5.
#include "d2client_callee.h"
// 1.13c D2Client+0x29F30
extern "C" uint32_t d2c_29f30(const D2cSvc& v, const uint8_t* unit, const uint8_t* tables /*[[0xCE474]]*/, const uint8_t* mask /*[0xCEE50]*/) {
    const uint32_t u = (uint32_t)(uintptr_t)unit;
    const uint32_t r = d2c_call(v, 0xc310, 0, 0, 0, 0, u, u);                                    // D2Common #10511
    if (!r) return 1;
    const int32_t idx = (int32_t)d2c_call(v, 0xc42a, 0, 0, 0, 0, u, r, 0xB2000000u | 0xd6ad4, 0x2cc);   // D2Common #10601
    if (idx < 0 || idx >= *(const int32_t*)(tables + 0xba0)) return 1;
    const uint32_t rec = *(const uint32_t*)(tables + 0xb98) + (uint32_t)idx * 0x23c;
    if (!rec) return 1;
    if (((const uint8_t*)(uintptr_t)rec)[7] & mask[0x1c]) return 1;
    if (unit) { const uint32_t m = *(const uint32_t*)(unit + 0x10); if (m == 1 || m == 5) return 1; }
    return 0;
}
