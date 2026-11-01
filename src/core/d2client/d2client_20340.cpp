// 1.13c D2Client+0x20340 -- four-neighbour overlay request: EAX = value (0 becomes 1), stdcall(unit, kind), ret 8.
// Kind 0xC3 needs a type-1 unit with +0x14 data whose byte +0x17 has bit 0. The 0x5C-byte request block (zeroed) holds 0x21,
// unit, kind (+0x10), the unit's x/y (+0x14/+0x18), 0 (+0x2C), the value (+0x30) and, unless kind is 0x141, the callback D2Client+0x73B60 (+0x54);
// for each neighbour (dx 0,1,0,-1; dy -1,0,1,0) the position is re-read, stored at +0x1C/+0x20 and 0xB30F0(ECX = block)
// runs twice with +0x58 = 0 then 1.
#include "cl_env.h"
extern "C" void d2c_20340(uint32_t v, uint32_t u, uint32_t kind, const cl::Env& e) {
    if (!v) v = 1;
    if (kind == 0xc3) {
        if (!u || e.R(u) != 1) return;
        uint32_t d = e.R(u + 0x14);
        if (!d || !(e.R8(d + 0x17) & 1)) return;
    }
    uint32_t q[23] = {};
    q[0] = 0x21; q[1] = u;
    q[5] = e.PD(u, 0xc3be, 0xc);
    q[6] = e.PD(u, 0xc2ec, 0x10);
    q[11] = 0; q[12] = v; q[4] = kind;
    if (kind != 0x141) q[21] = 0xB2000000u | 0x73b60u;   // callback D2Client+0x73B60 as a module tag (cf. HookEnv::handlerRva)
    static const int32_t dx[4] = {0, 1, 0, -1}, dy[4] = {-1, 0, 1, 0};
    for (int k = 0; k < 4; ++k) {
        uint32_t x = e.PD(u, 0xc3be, 0xc) + (uint32_t)dx[k];
        uint32_t y = e.PD(u, 0xc2ec, 0x10) + (uint32_t)dy[k];
        for (uint32_t s = 0; s < 2; ++s) {
            q[22] = s; q[7] = x; q[8] = y;
            e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        }
    }
}
