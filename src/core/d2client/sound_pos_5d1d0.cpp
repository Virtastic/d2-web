// 1.13c D2Client+0x5D1D0 -- positional sound update (EBX = sound entry; ).
// Callees go through the D2cHost seam, keyed by their retail RVA (import thunks D2sound#100xx included).
#include <cstdint>
#include <cstring>
#include <cmath>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
struct D2c5D1D0G {
    const uint8_t* tbl;   // 11C24C  sound-def table (0x8E-byte records)
    i32 cnt;              // 11C250
    i32 volA;             // E1374
    i32 volB;             // E1378
    i32 stereo;           // 11C280
    i32 ftolSse;          // 123710  (selects the 0xC9FF7 divide helper; same quotient)
    u32 k;                // D7580   float bits, scale applied to x,y,z
};
namespace {
inline u32 W(const void* p, u32 off) { u32 v; memcpy(&v, (const char*)p + off, 4); return v; }
inline float F(const void* p, u32 off) { float v; memcpy(&v, (const char*)p + off, 4); return v; }
inline float FB(u32 b) { float v; memcpy(&v, &b, 4); return v; }
inline u32 BF(float f) { u32 v; memcpy(&v, &f, 4); return v; }
inline u32 go(D2cHost h, u32 rva, u32 eax = 0, u32 ecx = 0, u32 edx = 0, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32* o = nullptr) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.ecx = ecx; c.edx = edx; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2;
    if (o) { c.out[0] = o[0]; c.out[1] = o[1]; }
    u32 r = h(&c);
    if (o) { o[0] = c.out[0]; o[1] = c.out[1]; }
    return r;
}
inline i32 div_(i32 a, i32 b) { return a / b; }   // idiv truncates toward zero, as the retail magic-multiply does
}  // namespace
extern "C" {
// P passed explicitly (wasm64: the 32-bit field at E+8 cannot hold a pointer).
EMSCRIPTEN_KEEPALIVE void d2c_5d1d0(D2cHost h, const D2c5D1D0G* g, uint8_t* e, const uint8_t* p) {
    if (!p) { u32 s = go(h, 0xBF92, 0x1e7); go(h, 0xBF80, 0, 0, 0, 0x6FB8359Fu, s); go(h, 0x3664, 0, 0, 0, 0xFFFFFFFFu); return; }
    i32 idx = (i32)W(p, 4);
    const uint8_t* slot = (g->tbl && idx >= 0 && idx < g->cnt) ? g->tbl + (uint32_t)idx * 0x8e : nullptr;
    u32 aB = go(h, 0x5DCE0, (u32)idx), bB = go(h, 0x5DC70, (u32)idx);   // far / near distance (float bits)
    float A = FB(aB), B = FB(bB);
    i32 v = (i32)W(p, 0x1c);
    if (slot && slot[0x53]) v = div_((i32)go(h, 0xD26A) * v, 100);
    i32 ebp = div_((i32)go(h, 0xD270) * v, 100);
    i32 pid = (i32)W(p, 4);
    if (g->volA != 100 && (pid < 0x1231 || pid > 0x125a) && (pid < 0x34 || pid > 0x47) && (pid < 1 || pid > 0xf))
        ebp = div_(g->volA * ebp, 100);
    if (g->volB != 100 && !(slot && slot[0x52])) ebp = div_(g->volB * ebp, 100);
    i32 e0 = (i32)W(e, 0), e1 = (i32)W(e, 4);
    i32 pan = 0;
    if (g->stereo) {
        i32 m = (i32)go(h, 0xD324);
        if (m == 1 || m == 2) {
            if ((i32)go(h, 0xD27C) != 0x32) {
                pan = div_((i32)go(h, 0xD27C) * 2 - 100, 3);
                if (e0 != 0 && pan > 0) ebp = div_((0xff - div_(pan * 0xff, 0x32)) * ebp, 0xff);
                if ((e1 & 2) && pan < 0) ebp = div_((div_(pan * 0xff, 0x32) + 0xff) * ebp, 0xff);
            }
        }
    }
    float x = F(p, 0xc), y = F(p, 0x10), z = F(p, 0x14);
    if (ebp == (i32)W(e, 0x18) && F(e, 0x1c) == F(p, 0x20) && !memcmp(p + 0xc, e + 0xc, 12)) return;
    // ponytail: x87 80-bit steps done in long double (IEEE quad on wasm); differs only when a result lies within 2^-64 of a ties/integer boundary
    long double L = sqrtl((long double)x * x + (long double)y * y);
    i32 vol = ebp;
    if (L > (long double)A) {
        long double q = ((long double)B - L) * (long double)ebp / ((long double)B - (long double)A);
        vol = (q > -9.2e18L && q < 9.2e18L) ? (i32)(uint32_t)(uint64_t)(int64_t)q : 0;   // _ftol 0xCACC4: low word of the truncated int64
    }
    const float kk = FB(g->k);
    u32 fx = BF(x * kk), fy = BF(y * kk), fz = BF(z * kk);
    i32 esi = div_((slot ? (i32)slot[0x3c] : 0) * vol, 255);   // null slot: retail reads address 0x3c (fault), not modelled
    u32 ent = (u32)e0;
    if (e1 == 0) {
        u32 o[2] = {(u32)ebp, fx};
        go(h, 0xD32A, 0, (u32)ebp, fx, fx, fy, fz, o);
        go(h, 0xD318, 0, ent, o[1]);
        esi = div_((i32)o[0] * esi, 255);
    }
    if (e1 & 2) go(h, 0xD300, 0, ent, 0, fx, fy, fz);
    go(h, 0xD2DC, 0, ent, 0, W(p, 0x20));
    go(h, 0xD2E2, 0, ent, (u32)esi);
    memcpy(e + 0xc, p + 0xc, 12);
    memcpy(e + 0x18, &ebp, 4);
    memcpy(e + 0x1c, p + 0x20, 4);
}
}
