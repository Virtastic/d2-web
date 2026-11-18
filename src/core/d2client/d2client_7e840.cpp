// 1.13c D2Client+0x7E840 -- clear the pending-select word (module 0x113ABA, dword); when under 100 ms have passed since the
// tick stored at 0x113AEC the message is handed on (dword 0x18 = 1, 0x1C = 0, 0xBE84) and 0x113AFF is cleared; later the
// state check at 0x7E6E0 decides and, when it passes, the message is handed on without the clear. Stack: message (ret 4);
// the tick count is passed in.
#include "cl_env.h"
extern "C" uint32_t d2c_7e840(uint32_t msg, uint32_t now, const cl::Env& e) {
    const uint32_t B = 0x6FAB0000u;
    e.W(B + 0x113aba, 0);
    if (now - e.R(B + 0x113aec) < 100u) {
        e.W(msg + 0x18, 1);
        e.W(msg + 0x1c, 0);
        const uint32_t v = e.H(0xbe84, msg);
        e.W(B + 0x113aff, 0);
        return v;
    }
    const uint32_t r = e.H(0x7e6e0);
    if (!r) return 0;
    e.W(msg + 0x1c, 0);
    e.W(msg + 0x18, 1);
    return e.H(0xbe84, msg);
}
