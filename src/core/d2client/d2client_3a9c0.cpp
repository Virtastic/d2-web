// 1.13c D2Client+0x3A9C0 -- key command: unless one of three panel flags (0xFADB0, 0xFADDC, 0xFADE4) is up, run the callee.
// A raised flag is what EAX holds on the way out.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3a9c0(uint32_t, const cl::Env& e) {
    static const uint32_t flags[3] = {0xFADB0, 0xFADDC, 0xFADE4};
    for (uint32_t f : flags) { const uint32_t v = clc::G(e, f); if (v) return v; }
    return e.H(0x90f60);
}
