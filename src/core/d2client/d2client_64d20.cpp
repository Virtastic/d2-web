// 1.13c D2Client+0x64D20 -- step the selected entry's cursor forward (stack = message, tail jump to Ordinal 511).
// g = pinned globals: +0 [11C058] entry index, +4 [11C060] entry array (0x550 bytes per entry). Entry: +0 mode, +0x110 / +0x114
// callbacks, +0x120 count, +0x124 cursor. The +0x110 callback (no arguments) may veto the step (result 0). Mode 1 wraps past the
// last entry to 0, mode 2 clamps at count-1 (unsigned compares). When the cursor changed the +0x114 callback runs (EDX = 0) and
// 0x88A70(0,0,0,0) is called with EBX = 1. Message +0x18 = 1 (EBX), +0x1C = 0. Code pointers are module addresses (base 0x6FAB0000).
#include "cl_env.h"
extern "C" void d2c_64d20(uint32_t msg, const cl::Env& e, uint32_t g) {
    const uint32_t B = 0x6FAB0000u;
    uint32_t s = e.R(g + 4) + e.R(g) * 0x550u;
    uint32_t old = e.R(s + 0x124), cb = e.R(s + 0x110);
    bool step = true;
    if (cb != 0) step = e.H(cb - B) != 0;
    if (step) {
        s = e.R(g + 4) + e.R(g) * 0x550u;
        uint32_t mode = e.R(s);
        if (mode == 1) {
            uint32_t v = e.R(s + 0x124) + 1u; e.W(s + 0x124, v);
            if (v > e.R(s + 0x120) - 1u) e.W(s + 0x124, 0);
        } else if (mode == 2) {
            uint32_t v = e.R(s + 0x124) + 1u; e.W(s + 0x124, v);
            uint32_t lim = e.R(s + 0x120) - 1u;
            if (v > lim) e.W(s + 0x124, lim);
        }
    }
    s = e.R(g + 4) + e.R(g) * 0x550u;
    if (old != e.R(s + 0x124)) {
        uint32_t cb2 = e.R(s + 0x114);
        if (cb2 != 0) e.H(cb2 - B, 0, 0, 0, 0, 0, 0, 0, 0);
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    }
    e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0); e.H(0xbe84, msg);
}
