// 1.13c D2Client+0x12090 -- message handler (stdcall, msg): busy gate, then the lookup in the table at 0x6FBA8B94 and the stamp of the
// last accepted word. The out word of the lookup (EAX = &local) is stored with the GetTickCount stamp when its low word is nonzero.
#include "d2client_sn18.h"
extern "C" uint32_t d2c_12090(uint32_t msg, const cl::Env& e) {
    if (e.R(0x6fbcc3a4)) { e.W(msg + 0x18, 0); return msg; }
    uint32_t out = 0;
    if (!e.R(0x6fba8bec)) { e.W(0x6fba8bec, 1); return sn18::tail(e, msg); }
    const uint32_t rec = e.R(e.R(0x6fba8b94) + e.R(0x6fbcc538) * 10);
    if (e.H(0xEA270, e.R(0x6fba8be8), sn18::R16(e, msg + 8), 0, 0, 0, (uint32_t)(uintptr_t)&out, rec)) {
        e.H(0x114F0, 1);
        e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, 2);
    }
    if (out & 0xffffu) {
        sn18::W16(e, 0x6fbcc54c, out);
        e.W(0x6fbcc550, e.H(sn18::kTick));
    }
    return sn18::tail(e, msg);
}
