// 1.13c D2Client+0x8CDD0 -- when the screen state word (module 0x11BC34) is 0xB: hand the message on (dword 0x18 = 1,
// 0x1C = 0, 0xBE84), then return the word at 0xFAD94 if set, else the result of the call at 0x59650. Stack: message (ret 4).
#include "cl_env.h"
extern "C" uint32_t d2c_8cdd0(uint32_t msg, const cl::Env& e) {
    const uint32_t B = 0x6FAB0000u;
    if (e.R(B + 0x11bc34) != 0xb) return 0;
    e.W(msg + 0x18, 1);
    e.W(msg + 0x1c, 0);
    e.H(0xbe84, msg);
    const uint32_t v = e.R(B + 0xfad94);
    if (v) return v;
    return e.H(0x59650);
}
