// 1.13c D2Client+0xBE190 -- no arguments, returns 0. A numeric value held in +0x11BBB0 is stepped by the delta two objects report.
// Object A (module word +0xFAD64; vtable +0x2C = kind, thiscall; vtable +0x30 = delta, one stack word 0) and the optional object B (+0xFAD68;
// vtable +0x2C reloads the stored value, vtable +0x38 is told the new one, one stack word). max = 0xC1C6(player +0x11BBFC, 0xF when the mode word
// +0xFAD5C is 4 else 0xE, 0). kind 1: value = min(value + delta, max) (unsigned); kind 2: delta > value gives 0, else value - delta; stored back to +0x11BBB0.
#include "cl_env.h"
extern "C" uint32_t d2c_be190(const cl::Env& e) {
    constexpr uint32_t kBase = 0x6FAB0000u;
    const auto G = [](uint32_t rva) { return kBase + rva; };
    uint32_t a = e.R(G(0xFAD64));
    const uint32_t kind = e.H(e.R(e.R(a) + 0x2c), 0, 0, 0, 0, 0, 0, a);
    a = e.R(G(0xFAD64));
    const uint32_t delta = e.H(e.R(e.R(a) + 0x30), 0, 0, 0, 0, 0, 0, a);
    uint32_t b = e.R(G(0xFAD68));
    if (b) e.W(G(0x11BBB0), e.H(e.R(e.R(b) + 0x2c), 0, 0, 0, 0, 0, 0, b));
    uint32_t cur = e.R(G(0x11BBB0));
    const uint32_t mx = e.H(0xc1c6, e.R(G(0x11BBFC)), e.R(G(0xFAD5C)) == 4 ? 0xf : 0xe, 0);
    if (kind == 1) {
        cur += delta;
        if (cur > mx) cur = mx;
    } else if (kind == 2) {
        if (delta > cur) cur = 0;
        else if (cur) cur -= delta;
    }
    b = e.R(G(0xFAD68));
    e.W(G(0x11BBB0), cur);
    if (b) e.H(e.R(e.R(b) + 0x38), cur, 0, 0, 0, 0, 0, b);
    return 0;
}
