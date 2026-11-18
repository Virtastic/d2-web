// 1.13c D2Client+0xb99d0 -- client missile: type-0x420 spawn at the unit's room point. ESI = unit, stack class, a, b
// (ret 0xC), returns 0xB30F0's result or 0. u == 0 returns 0. owner = u for unit types 0/1, else 0xA60D0(EAX = u)
// (0 returns 0). 0xA1340(EDI = &p, stack &q) fills the point pair and must return non-zero. Request word 0 = 0x420,
// 1 = owner, 2 = u, 4 = class, 7/8 = p/q, 11/12 = a/b; 0xB30F0(ECX = block).
#include "cl_env.h"
extern "C" uint32_t d2c_b99d0(uint32_t u, uint32_t cls, uint32_t a, uint32_t b, const cl::Env& e) {
    if (!u) return 0;
    uint32_t own = u;
    uint32_t ty = e.R(u);
    if (ty != 0 && ty != 1) { own = e.H(0xa60d0, 0, 0, 0, 0, 0, u); if (!own) return 0; }
    uint32_t pq[2] = { 0, 0 };   // retail's EDI out-word sits 4 bytes below the stack out-word
    uint32_t pp = (uint32_t)(uintptr_t)pq, qp = (uint32_t)(uintptr_t)(pq + 1);
    e.Ed(pp);
    uint32_t ok = e.H(0xa1340, qp);
    e.Edj(0xa1340, pp);
    if (!ok) return 0;
    uint32_t q[23] = {};
    q[0] = 0x420;
    q[1] = own;
    q[2] = u;
    q[4] = cls;
    q[7] = pq[0];
    q[8] = pq[1];
    q[11] = a;
    q[12] = b;
    return e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
