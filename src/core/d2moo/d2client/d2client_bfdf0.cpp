// 1.13c D2Client+0xBFDF0 -- no arguments, returns 1. Runs 0xBF720, then acts on the stored value v (+0x11BBB0) by the mode word (+0xFAD5C).
// v == 0: mode 2 runs 0x596A0 (ESI = v); every other mode returns. Mode above 4 returns.
// Modes 0/1: 0x14910(EAX = (player & ~0xFF) | 0x50, ECX = player +0xC or -1 without a player (+0x11BBFC), EDX = v).
// Mode 2: 0x596A0 (ESI = v). Modes 3/4: 0x149A0(EAX = 0x4F, ECX = 0x14 / 0x13, EDX = (v & 0xFFFF0000) | word +0x11BBB2, one stack word v).
// Modes 2, 3 and 4 then call 0x88A70 (EBX = 0xDD, four zero stack words).
#include "cl_env.h"
extern "C" uint32_t d2c_bfdf0(const cl::Env& e) {
    constexpr uint32_t kBase = 0x6FAB0000u;
    const auto G = [](uint32_t rva) { return kBase + rva; };
    e.H(0xbf720);
    const uint32_t v = e.R(G(0x11BBB0));
    const uint32_t mode = e.R(G(0xFAD5C));
    if (!v) {
        if (mode == 2) e.H(0x596a0, 0, 0, 0, 0, 0, 0, 0, 0, 0, v);
        return 1;
    }
    if (mode > 4) return 1;
    if (mode <= 1) {
        const uint32_t pl = e.R(G(0x11BBFC));
        e.H(0x14910, 0, 0, 0, 0, 0, (pl & ~0xffu) | 0x50u, pl ? e.R(pl + 0xc) : 0xffffffffu, v);
        return 1;
    }
    if (mode == 2) {
        e.H(0x596a0, 0, 0, 0, 0, 0, 0, 0, 0, 0, v);
    } else {
        const uint32_t ecx = mode == 3 ? 0x14u : 0x13u;
        const uint32_t edx = (v & 0xffff0000u) | (e.R(G(0x11BBB2)) & 0xffffu);
        e.H(0x149a0, v, 0, 0, 0, 0, 0x4f, ecx, edx);
    }
    e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 0xdd, 0);
    return 1;
}
