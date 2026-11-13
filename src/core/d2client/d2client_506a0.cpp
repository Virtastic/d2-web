// 1.13c D2Client+0x506A0 -- mouse-down on the panel toggle (stack = message, ret 4; Ordinal 511 pops the message).
// g = pinned globals, one dword each: +0 [11C414] mode, +4 [10ADA4] gate, +8 [11C30C] pressed flag (written), +0xC [119663].
// Ordinal 10031() == 2 with [10ADA4] set returns at once. Else the hit test is 0x4FE70() when Ordinal 10031() is 2, otherwise
// 0x4FEC0(EAX = mode, ECX = message x); zero sets message +0x18 = 0. A hit sets [11C30C] = 1, calls 0x88A70(0,0,0,0) with EBX = 4,
// 0x16360(); with mode 1, 0x46150() nonzero and [119663] == 1 it returns, else message +0x18 = 1, +0x1C = 0 and Ordinal 511.
#include "cl_env.h"
extern "C" void d2c_506a0(uint32_t msg, const cl::Env& e, uint32_t g) {
    uint32_t x = e.R(msg + 0xc) & 0xffff;
    if (e.H(0xd0ae) == 2 && e.R(g + 4) != 0) return;
    uint32_t mode = e.R(g);
    uint32_t ok = e.H(0xd0ae) == 2 ? e.H(0x4fe70) : e.H(0x4fec0, 0, 0, 0, 0, 0, mode, x);
    if (ok == 0) { e.W(msg + 0x18, 0); return; }
    e.W(g + 8, 1);
    e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
    e.H(0x16360);
    if (mode == 1 && e.H(0x46150) != 0 && e.R(g + 0xc) == 1) return;
    e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0); e.H(0xbe84, msg);
}
