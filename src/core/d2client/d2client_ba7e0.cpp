// 1.13c D2Client+0xBA7E0 (ecx = unit): looks the unit's 0xC322 id up in the 128-bucket unit hash (e.gl2 = DAT_6FBBAA08; bucket id & 0x7F, chain +0xE4, id at +0xC). Not found: tails to 0xB17C0. A found record whose type (+0) is not 2 is the error-log + _exit path (never vectored). When the 0xC4D2 value is 10: 0xB4FB0(EBX = unit, stack id), then the shared timers are stamped (e.gl: 0xBB9CC = tick, 0x50, 0x50, 0x190, then 0x11BEFC := 4). Always ends in 0xB17C0(ECX = unit).
// e.gl: 5 dwords = the 0x10B9CC quad then the 0x11BEFC word; e.gl2: the 128 bucket pointers.
#include "cl_env.h"
extern "C" uint32_t d2c_ba7e0(uint32_t u, const cl::Env& e) {
    const uint32_t id = e.H(0xc322, u);
    const uint32_t op = e.H(0xc4d2, u);
    uint32_t n = e.R(e.gl2 + (id & 0x7f) * 4);
    while (n && e.R(n + 0xc) != id) n = e.R(n + 0xe4);
    if (n) {
        if (e.R(n) != 2) {   // fatal: message 0x5a, log, _exit(-1)
            const uint32_t m = e.H(0xbf92, 0x5a);
            e.H(0xbf80, 0x6fb8359f, m);
            return e.H(0x3664, 0xffffffffu);
        }
        if (op == 10) {
            e.H(0xb4fb0, id, 0, 0, 0, 0, 0, 0, 0, u);
            e.W(e.gl, e.Tick(0xC0DE1000u)); e.W(e.gl + 4, 0x50); e.W(e.gl + 8, 0x50); e.W(e.gl + 12, 0x190); e.W(e.gl + 16, 4);
        }
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
