// 1.13c D2Client+0xA42E0 -- stdcall(msg): msg+0x18 = 1, +0x1C = 0, Storm 511(msg); q = [0x11BC00] (no q: the assert-and-exit path, 0xBF92 /
// 0xBF80 line 0x2C8 and _exit(-1)). q+0x54 (the panel state) below 5 returns the Storm result. Otherwise FUN 0xA3B50 (EAX = q, EDI = 0xD)
// != 0 returns it; else idx = q+0x44: out of [0, q+0x50) calls FUN 0xA3540 (EDX = q); in range, the entry callback at q + idx * 0x110 + 0x170
// (null returns 0) is called, and a result of exactly 1 goes on to FUN 0xBE2E0(); the return is the last callee's EAX.
#include "cl_cmd.h"
extern "C" uint32_t d2c_a42e0(uint32_t msg, const cl::Env& e) {
    e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0);
    uint32_t r = e.H(0xbe84, msg);
    uint32_t q = clc::G(e, 0x11BC00);
    if (!q) {
        uint32_t m = e.H(0xbf92, 0x2c8);
        e.H(0xbf80, 0xB2000000u | 0xD359F, m, 0x2c8);
        e.H(0x3664, 0xffffffffu);
        return 0;
    }
    if (e.R(q + 0x54) < 5) return r;
    uint32_t a = e.H(0xa3b50, 0, 0, 0, 0, 0, q);
    e.Ej(0xa3b50, 0xd);
    if (a) return a;
    int32_t idx = (int32_t)e.R(q + 0x44);
    if (idx < 0 || idx >= (int32_t)e.R(q + 0x50)) return e.H(0xa3540, 0, 0, 0, 0, 0, 0, 0, q);
    uint32_t fn = e.R(q + (uint32_t)idx * 0x110 + 0x170);
    if (!fn) return 0;
    uint32_t v = e.H(fn & 0xffffffu);
    if (v != 1) return v;
    return e.H(0xbe2e0);
}
