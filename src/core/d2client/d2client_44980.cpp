// 1.13c D2Client+0x44980 -- key-state update for the panel hotkey on a mouse/idle message (stack = message, ret 4).
// g = pinned globals, one dword each: +0 [104A60] (written), +4 [11C3E8], +8 [1046C4], +0xC [1046C8], +0x10 [11C3E0], +0x14 [11C3FC],
// +0x18 [11C3E4], +0x1C [11C2F8], +0x20 [10ADB4], +0x24 [11C3F0], +0x28 [11C2F4], +0x2C [DD5D0] table size (retail aborts when it is not 0x26;
// the vector never reaches that exit).
// [104A60] = message +8. Nonzero: Ordinal 10227(), then [10ADB4], [11C3F0], [11C2F8], [11C2F4] = 0, Ordinal 10153 with ECX = 0, message +0x18 = 0.
// Zero: when 0x398C0(EDX = 0x22, ESI = 1) is nonzero, 0x38E50(EDX = 1, ESI = 0x22) gives a key code, and a GetAsyncKeyState of its low word with
// bit 15 clear zeroes [11C3E8]. [1046C4] != 0x10 calls 0x44800. [1046C8] != 0x10 sets it 0x10, [11C3E0] = 0, [11C3FC] = 1, [11C3E4] = 0, [11C2F8] = 0.
// Message +0x18 = 0.
#include "cl_env.h"
extern "C" void d2c_44980(uint32_t msg, const cl::Env& e, uint32_t g) {
    uint32_t flag = e.R(msg + 8);
    e.W(g, flag);
    if (flag != 0) {
        e.H(0xbf8c);
        if (e.R(g + 0x2c) != 0x26) { uint32_t m = e.H(0xbf92, 0x139b); e.H(0xbf80, 0xB2000000u | 0xD359F, m); e.H(0x3664, 0xffffffffu); return; }
        e.W(g + 0x20, 0); e.W(g + 0x24, 0); e.W(g + 0x1c, 0); e.W(g + 0x28, 0);
        e.H(0xd3ea, 0, 0, 0, 0, 0, 0, 0);
        e.W(msg + 0x18, 0);
        return;
    }
    if (e.H(0x398c0, 0, 0, 0, 0, 0, 0, 0, 0x22, 0, 1) != 0) {
        uint32_t k = e.H(0x38e50, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0x22) & 0xffff;
        uint32_t st = e.H(0xcf0fc, k);
        if (((st >> 8) & 0x80) == 0) e.W(g + 4, 0);
    }
    if (e.R(g + 8) != 0x10) e.H(0x44800);
    if (e.R(g + 0xc) != 0x10) {
        e.W(g + 0xc, 0x10); e.W(g + 0x10, 0); e.W(g + 0x14, 1); e.W(g + 0x18, 0); e.W(g + 0x1c, 0);
    }
    e.W(msg + 0x18, 0);
}
