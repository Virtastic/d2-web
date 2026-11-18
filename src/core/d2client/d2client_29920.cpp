// 1.13c D2Client+0x29920: belt-use gate on three record bytes (+0x129, +0x12A, +0x11D); EAX record (never null on the
// path; retail asserts), stack (unit, item).
#include "d2client_callee.h"
// 1.13c D2Client+0x29920
extern "C" uint32_t d2c_29920(const D2cSvc& v, const uint8_t* rec, uint32_t s0, uint32_t s1) {
    if (rec[0x129] == 1 || rec[0x12a] == 1) { d2c_call(v, 0x7efa0, 0, 0, 0, 0, 0, s1); return 0; }
    if (rec[0x11d] == 1) return 1;
    d2c_call(v, 0x23200, 0, 0, 0, 0, 0, s0, 0x14);
    return 0;
}
