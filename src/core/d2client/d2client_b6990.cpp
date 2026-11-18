// 1.13c D2Client+0xb6990 -- client missile hit test against a target. ECX = missile unit, EDX = target unit.
// A null unit / class row / target returns 1. With Missiles row +0x6C set and a room, 0xA51F0(EAX = room, EDI = target)
// or else 0xA5B90(ESI = room, EDI = target) decides; on a hit, row +0x36 > 0 fires 0x6B300(target, 2, 0..0,
// EAX = that value); either way returns 1. Otherwise the target type gates: type 0 needs row +0x70 == 0; type 1
// needs D2Common 0xC20E(target) (row +0x70 == 2) or 0xC1D8(target) (else); a failed gate returns 0 (any other
// type returns 0). A passed gate posts a class-0xCC request (row +0x2C >= 0) and returns 1.
// EDI (the target) is not in the hook record: the capture's REGS cannot log it.
#include "cl_env.h"
extern "C" uint32_t d2c_b6990(uint32_t u, uint32_t t, const cl::Env& e) {
    if (!u) return 1;
    int32_t cid = (int32_t)e.R(u + 4);
    if (cid < 0) return 1;
    uint32_t row = e.Row(cid);
    if (!row || !t) return 1;
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (e.R(row + 0x6c) && room) {
        if (e.H(0xa51f0, 0, 0, 0, 0, 0, room) || e.H(0xa5b90, 0, 0, 0, 0, 0, 0, 0, 0, 0, room)) {
            int32_t s36 = e.R16s(row + 0x36);
            if (s36 > 0) e.H(0x6b300, t, 2, 0, 0, 0, (uint32_t)s36);
            return 1;
        }
    }
    uint32_t ty = e.R(t);
    if (ty == 0) {
        if (e.R(row + 0x70)) return 0;
    } else if (ty == 1) {
        uint32_t r = e.R(row + 0x70) == 2 ? e.H(0xc20e, t) : e.H(0xc1d8, t);
        if (!r) return 0;
    } else {
        return 0;
    }
    if (e.R16s(row + 0x2c) < 0) return 1;
    uint32_t q[23] = {};
    q[1] = room; q[2] = u;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = 0xcc;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    return 1;
}
