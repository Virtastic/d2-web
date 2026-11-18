// 1.13c D2Client+0xA4380 -- stdcall(msg): p = [0x11BC00]. While GetTickCount() < [p] + 0x50 the message is marked (msg+0x18 = 1, +0x1C = 0) and
// Storm 511(msg) is tail-called. Otherwise the same marking and Storm 511(msg), then p again (none: assert-and-exit, 0xBF92 / 0xBF80 line
// 0x1C2, _exit(-1)); p+0x54 below 5 returns p; a null callback at p+0x60 returns 0; else it is called and FUN 0xBE2E0() answers.
#include "cl_cmd.h"
extern "C" uint32_t d2c_a4380(uint32_t msg, const cl::Env& e) {
    uint32_t p = clc::G(e, 0x11BC00);
    if (p) {
        uint32_t t = e.H(0xcef5c);
        if (e.R(p) + 0x50u > t) {
            e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0);
            return e.H(0xbe84, msg);
        }
    }
    e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0);
    e.H(0xbe84, msg);
    p = clc::G(e, 0x11BC00);
    if (!p) {
        uint32_t m = e.H(0xbf92, 0x1c2);
        e.H(0xbf80, 0xB2000000u | 0xD359F, m, 0x1c2);
        e.H(0x3664, 0xffffffffu);
        return 0;
    }
    if (e.R(p + 0x54) < 5) return p;
    uint32_t fn = e.R(p + 0x60);
    if (!fn) return 0;
    e.H(fn & 0xffffffu);
    return e.H(0xbe2e0);
}
