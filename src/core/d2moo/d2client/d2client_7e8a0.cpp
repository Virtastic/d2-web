// 1.13c D2Client+0x7E8A0 -- looks up the first entry of the 114-entry table at 0x11A2E0 (stride 10: id dword, word at +4, flag
// dword at +6) with id 7 and flag 1; when its word is a valid code (<= 0xDF) equal to message dword 8 the code is the result,
// otherwise the message goes to 0x7E840. Stack: message (ret 4).
#include "cl_env.h"
extern "C" uint32_t d2c_7e8a0(uint32_t msg, const cl::Env& e) {
    const uint32_t B = 0x6FAB0000u + 0x11a2e0u;
    int hit = -1;
    for (int i = 0; i < 114; i++)
        if (e.R(B + 10 * i) == 7 && e.R(B + 10 * i + 6) == 1) { hit = i; break; }
    if (hit < 0) return e.H(0x7e840, msg);
    const uint32_t w = e.R(B + 10 * hit + 4) & 0xffff;
    if (w == 0xffff || w > 0xdf || e.R(msg + 8) != w) return e.H(0x7e840, msg);
    return w;
}
