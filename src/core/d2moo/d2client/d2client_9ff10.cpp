// 1.13c D2Client+0x9FF10 -- stdcall(msg): msg+0x18 = 1, +0x1C = 0, Storm 511(msg); x = word +0xC. UI mode [0x10B214] == 2: r = FUN 0x9FE50(x);
// r != 0 returns r; else FUN 0x8CED0 (ESI = msg) != 0 calls FUN 0x9FD70(). Other modes: FUN 0x973C0() != 0 returns it, else FUN 0x9FE50(x).
// The return is the last callee's EAX (0 when 0x8CED0 answered 0).
#include "cl_cmd.h"
extern "C" uint32_t d2c_9ff10(uint32_t msg, const cl::Env& e) {
    e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0);
    e.H(0xbe84, msg);
    uint32_t x = e.R(msg + 0xc) & 0xffff;
    if (clc::G(e, 0x10B214) == 2) {
        uint32_t r = e.H(0x9fe50, x);
        if (r != 0) return r;
        r = e.H(0x8ced0, 0, 0, 0, 0, 0, 0, 0, 0, 0, msg);
        if (r == 0) return 0;
        return e.H(0x9fd70);
    }
    uint32_t r = e.H(0x973c0);
    if (r != 0) return r;
    return e.H(0x9fe50, x);
}
