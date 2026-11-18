// 1.13c D2Client+0x2AFB0 -- queue-compatibility gate. ECX = the kind record `rec` (+0 type, +0x10 mode, +0xC4 flags), EAX = id,
// stdcall(a1 = target object or null, a2, a3), ret 0xC. gl+0 = DWORD 0x11C2F4, gl+4 = DWORD 0x11C2F8.
// Allowed (type 0: mode 0 or 0x11; type 1: mode 0 or 0xC) or flags & 0x10000: look the id up (0x9D630, EDX = id); a miss, or a
// class mismatch with the target (a1 + 0xC, or -1) through 0x4FD90(EDX = found + 0x10, ESI = target, stack 1), latches
// 0x11C2F8 = 1 and 0x11C2F4 = 0 and returns 0; else 0x2A7C0(EAX = a1, EBX = a3, stack a2, 0) and return 0.
// Otherwise flags & 2 set: 0x29E80(EAX = a2, ECX = a1) == 0 returns 1, else 0x2AA40(EAX = a1, stack a2, a3, id) and return 0;
// flags & 2 clear: the same latch and 0.
#include "cl_env.h"
extern "C" uint32_t d2c_2afb0(uint32_t rec, uint32_t id, uint32_t a1, uint32_t a2, uint32_t a3, const cl::Env& e) {
    uint32_t flags = e.R(rec + 0xc4);
    bool direct = (flags & 0x10000u) != 0;
    if (!direct) {
        uint32_t t = e.R(rec), k = e.R(rec + 0x10);
        direct = (t == 0 && (k == 0 || k == 0x11)) || (t == 1 && (k == 0 || k == 0xc));
    }
    if (direct) {
        uint32_t tgt = a1 ? e.R(a1 + 0xc) : 0xffffffffu;
        uint32_t f = e.H(0x9d630, 0, 0, 0, 0, 0, 0, 0, id, 0, 0);
        if (f) {
            uint32_t v = e.R(f + 0x10);
            if (tgt == v || e.H(0x4fd90, 1, 0, 0, 0, 0, 0, 0, v, 0, tgt)) {
                e.H(0x2a7c0, a2, 0, 0, 0, 0, a1, 0, 0, a3, 0);
                return 0;
            }
        }
    } else if (flags & 2u) {
        if (!e.H(0x29e80, 0, 0, 0, 0, 0, a2, a1, 0, 0, 0)) return 1;
        e.H(0x2aa40, a2, a3, id, 0, 0, a1, 0, 0, 0, 0);
        return 0;
    }
    e.W(e.gl + 4, 1);
    e.W(e.gl, 0);
    return 0;
}
