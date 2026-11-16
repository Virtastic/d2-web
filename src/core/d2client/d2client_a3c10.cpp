// 1.13c D2Client+0xA3C10 -- mouse message handler of the small button at the bottom centre (stdcall, one stack word = the message record P, ret 4,
// void; EAX modelled). x = u16[P+0xC], y = u16[P+0xE]; W / H = the screen size words at 0xDBC48 / 0xDBC4C. The button is hit when the flag at
// 0xFADD4 is set and x is in (W/2-0x4A, W/2+0x47) and y in (H-0x45, H-0x32). Outside it: P[+0x18] = 1, P[+0x1C] = 0, 0xBE84(P); the panel block
// (0x11BC00, else the 0x123 fatal path) with state word +0x10 not 1 offers the message through listener slot +0x10 (first answer 1 ends it);
// with state 1 the tick (import 0xCEF5C) below block[0] + 0x50 returns, else 0xA34B0 (EAX = P) answering the index at +0x48, in [0, count at +0x50),
// with a callback at +0x170 + 0x110 * the row at +0x44, runs it (nonzero answer leaves state alone); every other way clears state (+0x10 := 0).
// Inside the button only the state test: block null returns, state 1 clears it.
#include "cl_walk.h"
extern "C" uint32_t d2c_a3c10(uint32_t P, const cl::Env& e) {
    const uint32_t p = clc::G(e, 0x11bc00);
    const int32_t x = (int32_t)pa4::P16(e, P + 0xc), y = (int32_t)pa4::P16(e, P + 0xe);
    const int32_t W = (int32_t)clc::G(e, 0xdbc48), H = (int32_t)clc::G(e, 0xdbc4c);
    const bool hit = clc::G(e, 0xfadd4) != 0 && x > W / 2 - 0x4a && x < W / 2 + 0x47 && y < H - 0x32 && y > H - 0x45;
    if (hit) {
        if (!p) return (uint32_t)(H - 0x45);
        if (e.R(p + 0x10) == 1) e.W(p + 0x10, 0);
        return (uint32_t)(H - 0x45);
    }
    uint32_t eax = pa4::fin(e, P);
    if (!p) {
        pa4::assertAt(e, 0x123);
        return eax;
    }
    if (e.R(p + 0x10) != 1) {
        uint32_t a = eax;
        for (uint32_t n = e.R(p + 0xb08); n; n = e.R(n + 4)) {
            const uint32_t o = e.R(n);
            if (!o) continue;
            a = e.H(e.R(e.R(o) + 0x10) & 0xFFFFFFu, P, 0, 0, 0, 0, 0, o);
            if (a == 1) break;
        }
        return a;
    }
    const uint32_t t = e.H(0xcef5c);
    if (e.R(p) + 0x50u > t) return t;
    const uint32_t ix = e.H(0xa34b0, 0, 0, 0, 0, 0, P);
    uint32_t r = ix;
    if (e.R(p + 0x48) == ix && (int32_t)ix >= 0 && (int32_t)ix < (int32_t)e.R(p + 0x50)) {
        const uint32_t fp = e.R(p + e.R(p + 0x44) * 0x110 + 0x170);
        if (fp) {
            r = e.H(fp & 0xFFFFFFu);
            if (r != 0) return r;
        } else {
            r = 0;
        }
    }
    e.W(p + 0x10, 0);
    return r;
}
