// 1.13c D2Client+0x2D000 (542 B, ret 0x10, EAX = u, EBX = scale; stack s0, dst, s2, off): lo = thunk 0xC85C(s0, s2, u, 0) >> 8 and
// hi = thunk 0xC7F0(s0, s2, u, 0) >> 8 (arithmetic). Each is scaled: v + f(v) + off with f the signed v*scale/100 ladder (the retail branch
// choice by magnitude: 32-bit product, 64-bit alldiv, or the divide-first form). a == 0 && b == 0 returns 0. Else a != 0 && a == b gives
// "%d" (a < 0) or "+%d"; otherwise "%d-%d". strcat dst str(0x10A0), local, str(0xF9E); returns 1.
#include "d2client_2d000.h"
#include "d2client_wfmt.h"
namespace {
inline int32_t div100(int32_t v) { return v / 100; }
inline int32_t scaled(int32_t v, int32_t scale) {
    const int32_t lowv = v & (int32_t)0xfffffff0, lows = scale & (int32_t)0xfffffff0;
    if (v > 0x100000) {
        if (lowv < 0x640) return (int32_t)((int64_t)v * (int64_t)scale / 100);
        return (int32_t)((uint32_t)div100(v) * (uint32_t)scale);
    }
    if (scale > 0x10000) {
        if (lows < 0x640) return (int32_t)((int64_t)v * (int64_t)scale / 100);
        return (int32_t)((uint32_t)div100(scale) * (uint32_t)v);
    }
    return div100((int32_t)((uint32_t)v * (uint32_t)scale));
}
}
extern "C" uint32_t d2c_port_2d000(uint16_t* dst, uint32_t u, int32_t scale, uint32_t s0, uint32_t s2, int32_t off, const D2c2D000Env& e) {
    using namespace d2client_wfmt;
    int32_t lo = (int32_t)e.c85c(s0, s2, u, 0) >> 8;
    int32_t hi = (int32_t)e.c7f0(s0, s2, u, 0) >> 8;
    int32_t a = (int32_t)((uint32_t)lo + (uint32_t)scaled(lo, scale) + (uint32_t)off);
    int32_t b = (int32_t)((uint32_t)hi + (uint32_t)scaled(hi, scale) + (uint32_t)off);
    if (a == 0 && b == 0) return 0;
    Buf o;
    if (a != 0 && a == b) { if (a >= 0) put(o, '+'); putInt(o, a); }
    else { putInt(o, a); put(o, '-'); putInt(o, b); }
    uint16_t local[64] = {}; widen(o, local);
    e.u.strcat(dst, e.u.str(0x10A0));
    e.u.strcat(dst, local);
    e.u.strcat(dst, e.u.str(0xF9E));
    return 1;
}
