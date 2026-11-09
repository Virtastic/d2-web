// 1.13c D2Client+0xb9ff0 -- client missile: frame-scheduled smoke, milestone effects and a nearby-target trail.
// ECX = unit; every path ends in 0xB17C0(ECX = u) and returns its value. owner = 0xA60D0(EAX = u) (0: straight to
// the tail). f = 0xC406(u). The cadence table at D2Client+0xD35D4 ({limit, period, on} x4, then {0, 0x40, 0}) is
// scanned for the first entry with f > limit; when f % period == 0 (signed) and on: one discarded seed step on
// u +0x20, request word 0 = 2, 1 = owner, 2 = u, 4 = 0x272, 7/8 = seed % 40 - 20 each, 11/12 = 0xC388/0xC3FA(u);
// 0xB30F0(ECX = block), a child gets 0xC448(child, u +0xC). 0xC448(u, f < 0xD2 ? 2 : 1). f = 0x145/0x177/0x1A9
// call 0xAFF10(EDX = owner, EAX = id; 0x27A/0x279/0x278, x, y, id = 0xC388(u), 0xC3FA(u)) and f = 0x1C2 the same with
// 0x277 and x+1, y+1 (y read before x, then 0xC3FA before 0xC388). When 0xC4BA(u) != 0 and
// t = 0xC334(0xC1F0(u), 1, 0x209) != 0: two seed steps a, b; 0xB3B40(EAX = u, EBX = t.y + b % 10 - 5; t.x + a % 10 - 5).
#include "cl_env.h"
namespace {
struct Cad { int32_t lim, per, on; };
constexpr Cad kCad[5] = { { 0x1a4, 0x40, 0 }, { 0x1b3, 0x14, 1 }, { 0x1d6, 8, 1 }, { 0x10e, 5, 1 }, { 0, 0x40, 0 } };   // D2Client+0xD35D4 (.rdata)
}
extern "C" uint32_t d2c_b9ff0(uint32_t u, const cl::Env& e) {
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!own) return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
    int32_t f = (int32_t)e.H(0xc406, u);
    uint32_t i = 0;
    while (i < 4 && f <= kCad[i].lim) ++i;
    uint32_t sd = u + 0x20;
    if (f % kCad[i].per == 0 && kCad[i].on) {
        e.Lcg(sd);
        uint32_t q[23] = {};
        q[1] = own;
        uint32_t dx = e.Lcg(sd) % 40u;
        q[0] = 2;
        q[2] = u;
        q[4] = 0x272;
        q[7] = dx - 20;
        q[8] = e.Lcg(sd) % 40u - 20;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (m) e.H(0xc448, m, e.R(u + 0xc));
    }
    e.H(0xc448, u, f < 0xd2 ? 2u : 1u);
    uint32_t cls = 0;
    switch (f) {
        case 0x145: cls = 0x27a; break;
        case 0x177: cls = 0x279; break;
        case 0x1a9: cls = 0x278; break;
        case 0x1c2: cls = 0x277; break;
    }
    if (cls) {
        uint32_t y = e.PD(u, 0xc2ec, 0x10);
        uint32_t x = e.PD(u, 0xc3be, 0xc);
        uint32_t lv = e.H(0xc3fa, u);
        uint32_t id = e.H(0xc388, u);
        if (cls == 0x277) { ++x; ++y; }
        e.H(0xaff10, cls, x, y, id, lv, id, 0, own);
    }
    if (e.H(0xc4ba, u)) {
        uint32_t t = e.H(0xc334, e.H(0xc1f0, u), 1, 0x209);
        if (t) {
            uint32_t a = e.Lcg(sd) % 10u;
            uint32_t b = e.Lcg(sd) % 10u;
            uint32_t tx = e.PD(t, 0xc3be, 0xc) + a - 5;
            uint32_t ty = e.PD(t, 0xc2ec, 0x10) + b - 5;
            e.H(0xb3b40, tx, 0, 0, 0, 0, u, 0, 0, ty);
        }
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
