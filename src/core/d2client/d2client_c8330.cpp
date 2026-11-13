// 1.13c D2Client+0xC8330 (ecx = unit, ret 4 with the stack word unused): 0xC310(unit) is the handle (0: return 0); 0x2A0B0(EAX = unit); a unit without a path (+0x2C) is the error-log + _exit path (never vectored). 0xC2DA(path) gives the owner unit (0: return 0); the local player (e.self) additionally needs a non-zero 0xC5F8(owner). Then 0xCB50(handle, x), 0xCB7A(handle, y) with the owner's position (static path read for types 2/4/5, else 0xC3BE/0xC2EC of the path) and 0xCB8C(handle, owner+4). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_c8330(uint32_t u, const cl::Env& e) {
    const uint32_t h = e.H(0xc310, u);
    if (!h) return 0;
    e.H(0x2a0b0, 0, 0, 0, 0, 0, u);
    uint32_t p = e.R(u + 0x2c);
    if (!p) {   // 0x946 message, log, _exit(-1)
        const uint32_t m = e.H(0xbf92, 0x946);
        e.H(0xbf80, 0x6fb8359f, m);
        e.H(0x3664, 0xffffffffu);
        return 0;
    }
    const uint32_t o = e.H(0xc2da, p);
    if (!o) return 0;
    if (u == e.self && !e.H(0xc5f8, o)) return 0;
    e.H(0xcb50, h, e.PD(o, 0xc3be, 0xc));
    e.H(0xcb7a, h, e.PD(o, 0xc2ec, 0x10));
    e.H(0xcb8c, h, e.R(o + 4));
    return 1;
}
