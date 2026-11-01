// 1.13c D2Client+0x2A750: local action dispatch on the action record's type word (ECX owner, EDX record, two stack words).
#include "d2client_callee.h"
// 1.13c D2Client+0x2A750
extern "C" uint32_t d2c_2a750(const D2cSvc& v, uint32_t ecx, const uint32_t* rec, uint32_t s0, uint32_t s1) {
    const uint32_t edx = (uint32_t)(uintptr_t)rec, t = rec[0];
    switch (t) {
        case 0: return d2c_call(v, 0x19830, t, ecx, edx, 0, 0, s0, s1);            // tail jump, the callee pops both words
        case 1: d2c_call(v, 0x4dc70, t, ecx, edx, 0, 0, s0); return 1;
        case 2: d2c_call(v, 0x25ce0, s0, ecx, edx, 0, 0); return 1;
        case 4: { const uint32_t* p = (const uint32_t*)(uintptr_t)s0; d2c_call(v, 0x81d30, ecx, 0, 0, 0, edx, p[0], p[1]); return 1; }
        default: return 1;
    }
}
