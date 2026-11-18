// 1.13c D2Client+0x1ED80 -- message handler (stdcall, msg): the common tail, then 0x1E7E0(1) and the GetTickCount stamp (a dword at the
// unaligned address 0x6FBD3399). Returns the tick.
#include "d2client_sn18.h"
extern "C" uint32_t d2c_1ed80(uint32_t msg, const cl::Env& e) {
    sn18::tail(e, msg);
    e.H(0x1E7E0, 1);
    const uint32_t t = e.H(sn18::kTick);
    e.W(0x6fbd3399, t);
    return t;
}
