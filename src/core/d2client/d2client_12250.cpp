// 1.13c D2Client+0x12250 -- message handler (stdcall, msg): busy gate, then 0x114F0(1), the flag 0x6FBCC508 := 1, 0x11A80(x, y) with the
// words at msg+0xC and msg+0xE, and the tail.
#include "d2client_sn18.h"
extern "C" uint32_t d2c_12250(uint32_t msg, const cl::Env& e) {
    if (e.R(0x6fbcc3a4)) { e.W(msg + 0x18, 0); return msg; }
    const uint32_t x = sn18::R16(e, msg + 0xc), y = sn18::R16(e, msg + 0xe);
    e.H(0x114F0, 1);
    e.W(0x6fbcc508, 1);
    e.H(0x11A80, x, y);
    return sn18::tail(e, msg);
}
