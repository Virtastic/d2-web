// 1.13c D2Client+0x7E6E0 (340 B, no args, plain ret): panel_close_request. Returns 1 when the
// close went through. Module globals by address (all dword reads, some unaligned): ga = window 0x113A60..0x113B03 (0x3A60 box,
// 0x3A7C, 0x3A80, 0x3A84, 0x3A88, 0x3AC2 the 0x486A0 EAX, 0x3ACA, 0x3AFF the callback argument), gcb = 0x11BF94 (callback,
// fastcall: ECX = 0x11BF98, EDX = 1, one stack word; logged at the RVA it points to), gd = 0xDD5D4, gt = 0xFAD50 (resume tick),
// gq = 0x123448. now = GetTickCount.
#include "cl_env.h"
extern "C" uint32_t d2c_7e6e0(uint32_t ga, uint32_t gcb, uint32_t gd, uint32_t gt, uint32_t gq, uint32_t now, const cl::Env& e) {
    const uint32_t kFile = 0xB20D4BDCu;
    auto G = [&](uint32_t off) { return e.R(ga + off - 0x3a60); };
    auto S = [&](uint32_t off, uint32_t v) { e.W(ga + off - 0x3a60, v); };
    e.W(ga + 0x1c, 1);   // 0x113A7C
    if (G(0x3aca) == 0 && G(0x3a80) != 1) {
        uint32_t r = 0;
        if (G(0x3a84) == 1) {
            if (G(0x3a88) == 1) { S(0x3aff, 0); return 0; }
            // G(0x3a60) == 0 is the fatal D2Lang 0x943 message and _exit(-1); the vectors keep the box set
            e.H(0x7bf40);
            e.H(0xbf86, 0x945, 0, 0, 0, 0, 0, G(0x3a60), kFile);
            S(0x3a60, 0); S(0x3a84, 0); S(0x3a88, 0);
            r = 1;
        }
        S(0x3aff, 0);
        return r;
    }
    if (const uint32_t fp = e.R(gcb)) {
        e.H(0x7cef0);
        if (e.R(gd)) e.W(gt, now + 0x64);
        const uint32_t ok = e.H(fp - 0x6fab0000u, G(0x3aff), 0, 0, 0, 0, 0, e.R(gcb + 4), 1);
        if (ok) { S(0x3aff, 0); return 1; }
        if (G(0x3aca)) e.H(0x486a0, 0, 0, 0, 0, 0, G(0x3ac2));
    } else {
        if (G(0x3aca)) e.H(0x486a0, 0, 0, 0, 0, 0, G(0x3ac2));
        else e.H(0x7cef0);
        if (e.R(gd)) e.W(gt, now + 0x64);
    }
    S(0x3aca, 0);
    if (e.R(gq) == 1) {
        e.W(gq, 0);
        const uint32_t v = e.H(0xd0b4);
        e.H(0x3cc60, v, 0xB20E1388u, 0, 0, 0, 7);
    }
    S(0x3aff, 0);
    return 1;
}
