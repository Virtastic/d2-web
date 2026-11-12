// 1.13c D2Client+0xb9a80 -- spawn helper for request type 0x521. EAX = unit, eight stack words (ret 0x20), returns 0xB30F0's value.
// u == 0: 0. Unit type not 0/1: owner = 0xA60D0(EAX = u), 0 when none; else owner = u.
// s3 or s4 zero: 0xA1340(EBX = u, EDI = &s3, stack &s4) fills both, failure returns 0; either still zero returns 0. s7 == 0 becomes 0x1E.
// The 0x5C-byte request block (zeroed): 0x521, owner, s0 (+0x10), 0x1630(ECX = u) + s1 (+0x14), 0x1660(ECX = u) + s2 (+0x18),
// s3 (+0x1C), s4 (+0x20), s7 (+0x24), s5 (+0x2C), s6 (+0x30); then 0xB30F0(ECX = block).
#include <cstdint>
#include "cl_env.h"
extern "C" uint32_t d2c_b9a80(uint32_t u, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5,
                              uint32_t s6, uint32_t s7, const cl::Env& e) {
    if (!u) return 0;
    uint32_t t = e.R(u), o = u;
    if (t != 0 && t != 1) { o = e.H(0xa60d0, 0, 0, 0, 0, 0, u); if (!o) return 0; }
    if (!s3 || !s4) {
        e.Ed((uint32_t)(uintptr_t)&s3);
        if (!e.H(0xa1340, (uint32_t)(uintptr_t)&s4, 0, 0, 0, 0, 0, 0, 0, u)) return 0;
    }
    if (!s3 || !s4) return 0;
    if (!s7) s7 = 0x1e;
    uint32_t q[23] = {};
    q[0] = 0x521; q[1] = o;
    q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u) + s1;
    q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u) + s2;
    q[7] = s3; q[8] = s4; q[9] = s7; q[11] = s5; q[12] = s6; q[4] = s0;
    return e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
