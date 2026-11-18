// 1.13c D2Client+0x70000: stdcall(a1), ret 4. Called with a screen rect (u16 at +0xE = the bottom edge): when it lies below the
// bottom limit minus 0x31, the open-panel flag 0xFAD94 is cleared and, if it was set, the panel-close work runs (0x6F4A0, then, when
// no panel flag is up, the two 0x11C434 / 0x123498 messages through 0x3CC60 / 0x3CCC0, and the 0xFADA8 message).
#include <cstdint>
#include "cl_env.h"
namespace { constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; } constexpr uint32_t kT = 0xB2000000u; }
extern "C" void d2c_70000(uint32_t a1, const cl::Env& e) {
    if ((int32_t)(e.R(a1 + 0xC) >> 16) <= (int32_t)e.R(G(0xDBC4C)) - 0x31) return;
    const uint32_t was = e.R(G(0xFAD94));
    e.H(0xBF8C);
    if (e.R(G(0xDD5D0)) != 0x26) {              // table-count check: error box then exit(-1)
        const uint32_t m = e.H(0xBF92, 0x139B);
        e.H(0xBF80, kT | 0xD359F, m);
        e.H(0x3664, 0xFFFFFFFFu);
        return;
    }
    e.W(G(0xFAD94), 0);
    if (!was) return;
    e.H(0x6F4A0);
    static const uint32_t kFlags[9] = { 0xFADB0, 0xFADDC, 0xFADE4, 0xFADE8, 0xFADEC, 0xFADF0, 0xFADF4, 0xFADF8, 0xFAE00 };
    bool clear = true;
    for (uint32_t f : kFlags) if (e.R(G(f))) { clear = false; break; }
    if (clear) {
        e.W(G(0x11C434), 1);
        if (!e.R(G(0x12349C))) {
            e.W(G(0x12349C), 1);
            if (e.R(G(0x123498))) {
                const uint32_t r = e.H(0xD0B4);
                e.H(0x3CC60, r, kT | 0xF51EC, 0, 0, 0, 2);
                e.W(G(0x123498), 0);
            }
            const uint32_t r = e.H(0xD0B4);
            e.H(0x3CCC0, r, kT | 0xF51BC, 0, 0, 0, 4);
        }
    }
    if (e.R(G(0xFADA8))) {
        const uint32_t r = e.H(0xD0B4);
        e.H(0x3CCC0, r, kT | 0xD0780, 0, 0, 0, 5);
    }
}
