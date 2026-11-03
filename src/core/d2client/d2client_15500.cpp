// 1.13c D2Client+0x15500: fastcall (ECX owner, EDX table index), one stack argument (ret 4). Disassembly-derived (the decompile hides the
// register and stack arguments); callee records go through cl::Env::H.
// Gate: 0xA0A30(eax owner) != 0, row = *(dt+0xB98) + index * 0x23C (index in [0, dt+0xBA0)), h = 0xC310(owner) != 0.
// A = 0xC316(owner, row[0x64], index, arg); t = 0xA5F70(). t != 0: obj = t, seed = 0xA1AD0(eax 3, edx A; owner, 0, 0);
// t == 0: obj = 0xA1A50(eax 3, ecx -1, edx A; owner, 0, 0) with the seed written through EDI (the initial seed is 0).
// B = 0xC316(owner, row[0x140], index, arg), C = 0xC316(owner, row[0x138], index, arg); clamp = min(max(seed, min(B, C)), C) (signed);
// 0xCB50(h, clamp). obj != 0: 0xCB7A(h, obj[0]), 0xCB8C(h, obj[0xC]), then 0xC478(owner[0x2C], x, y, 0) with y = 0x1660(ecx obj) and
// x = 0x1630(ecx obj) called in that order; obj == 0: 0xCB7A(h, 1), 0xCB8C(h, -1). Returns 1 past the gates.
#include "cl_env.h"
extern "C" uint32_t d2cl_15500(uint32_t owner, int32_t index, uint32_t arg, const cl::Env& e) {
    if (!e.H(0xa0a30, 0, 0, 0, 0, 0, owner)) return 0;
    if (index < 0 || index >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    const uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)index * 0x23c;
    if (!row) return 0;
    const uint32_t h = e.H(0xc310, owner);
    if (!h) return 0;
    const uint32_t A = e.H(0xc316, owner, e.R(row + 0x64), (uint32_t)index, arg);
    const uint32_t t = e.H(0xa5f70);
    int32_t seed = 0;
    uint32_t obj;
    if (t) {
        obj = t;
        seed = (int32_t)e.H(0xa1ad0, owner, 0, 0, 0, 0, 3, 0, A);
    } else {
        e.Ed((uint32_t)(uintptr_t)&seed);
        obj = e.H(0xa1a50, owner, 0, 0, 0, 0, 3, 0xffffffffu, A);
    }
    const int32_t B = (int32_t)e.H(0xc316, owner, e.R(row + 0x140), (uint32_t)index, arg);
    const int32_t C = (int32_t)e.H(0xc316, owner, e.R(row + 0x138), (uint32_t)index, arg);
    int32_t v = B > C ? C : B;
    if (seed > v) v = seed;
    if (v >= C) v = C;
    e.H(0xcb50, h, (uint32_t)v);
    if (obj) {
        e.H(0xcb7a, h, e.R(obj));
        e.H(0xcb8c, h, e.R(obj + 0xc));
        const uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, obj);
        const uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, obj);
        e.H(0xc478, e.R(owner + 0x2c), x, y, 0);
    } else {
        e.H(0xcb7a, h, 1);
        e.H(0xcb8c, h, 0xffffffffu);
    }
    return 1;
}
