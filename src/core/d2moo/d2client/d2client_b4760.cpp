// 1.13c D2Client+0xb4760 -- client missile: the countdown spark of a counting unit, then its tick (0xB17C0).
// ECX = unit. n = D2Common counter (0xC406); the step table at 0xD364C (thresh, period, count; four rows and a
// fallback) picks the period; n % period == 0 spawns `count` class-0x15F sparks at +-(2..5)*2 around the unit, each
// handed the unit's guid (0xC448). n < 25 / n < 75 then set the unit's mode 3 / 2 (0xC448). At n <= 1 the owner
// (0xC4BA id, looked up in the client unit hash at 0x10AA08, chain at +0xE4) must be a type-2 object: it gets
// overlay 0xB6 (0x6B300) and, once, light 0x3C (0xA8C20) at +0x64. gl = the 128-bucket hash (0x10AA08).
#include "cl_env.h"
namespace {
const int32_t kStep[5][3] = { { 350, 10, 1 }, { 300, 5, 1 }, { 200, 4, 1 }, { 175, 3, 1 }, { 0, 100, 0 } };   // 0xD364C
int32_t off5(uint32_t r) { int32_t d = (int32_t)(r % 10) - 5; return (d < 0 ? -d : d) < 2 ? 5 : d; }
}
extern "C" void d2c_b4760(uint32_t u, uint32_t gl, const cl::Env& e) {
    int32_t n = (int32_t)e.H(0xc406, u);
    int i = 0;
    while (i < 4 && n <= kStep[i][0]) ++i;
    if (n % kStep[i][1] == 0) {
        uint32_t px = e.PD(u, 0xc3be, 0xc);
        uint32_t py = e.PD(u, 0xc2ec, 0x10);
        uint32_t q[23] = {};
        q[0] = 1;
        q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[4] = 0x15f;
        e.H(0xc388, u);
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        for (int32_t k = 0; k < kStep[i][2]; ++k) {
            int32_t dx = off5(e.Lcg(u + 0x20));
            int32_t dy = off5(e.Lcg(u + 0x20));
            q[5] = px + (uint32_t)dx * 2;
            q[6] = py + (uint32_t)dy * 2;
            uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            if (m) e.H(0xc448, m, e.R(u + 0xc));
        }
    }
    if (n < 0x19) e.H(0xc448, u, 3);
    else if (n < 0x4b) e.H(0xc448, u, 2);
    if (n <= 1) {
        uint32_t id = e.H(0xc4ba, u);
        uint32_t p = e.R(gl + (id & 0x7f) * 4);
        while (p && e.R(p + 0xc) != id) p = e.R(p + 0xe4);
        if (p) {
            if (e.R(p) != 2) {
                e.H(0xbf80, 0x6fb8359f, e.H(0xbf92, 0x5a), 0x5a);
                e.H(0x3664, 0xffffffffu);
                return;
            }
            e.H(0x6b300, p, 3, 0, 0, 0, 0xb6);
            if (!e.R(p + 0x64)) e.W(p + 0x64, e.H(0xa8c20, 0x3c, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 2));
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
