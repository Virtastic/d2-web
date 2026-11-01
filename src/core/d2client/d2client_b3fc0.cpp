// 1.13c D2Client+0xb3fc0 -- client missile: one scattered ring piece of class `cls` (Missiles row speed must be 25);
// ECX = room, EAX = source unit, stack (cls), ret 4. Direction (a, b) from D2Common 10xxx (0xC45A out-params); the two
// 0xC2F8 calls normalise (-25a, -25b) and (a, b) in place before they become the child's velocity / acceleration.
#include "cl_env.h"
extern "C" void d2c_b3fc0(uint32_t room, uint32_t u, uint32_t cls, const cl::Env& e) {
    uint32_t ab[2] = { 0, 0 };   // [esp+0x10], [esp+0x14]
    e.H(0xc45a, room, (uint32_t)(uintptr_t)&ab[0], (uint32_t)(uintptr_t)&ab[1]);
    if (ab[0] == 0 && ab[1] == 0) {
        e.H(0xbf80, 0x6fb8359f, e.H(0xbf92, 0x1850), 0x1850);
        e.H(0x3664, 0xffffffffu);
        return;
    }
    uint32_t q[23] = {};
    q[0] = 1; q[1] = room; q[4] = cls;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    uint32_t px = e.PD(u, 0xc3be, 0xc);
    q[5] = e.Lcg(u + 0x20) % 0x14 + px - 10;
    uint32_t py = e.PD(u, 0xc2ec, 0x10);
    q[6] = e.Lcg(u + 0x20) % 0x14 + py - 10;
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (!m) return;
    uint32_t row = e.Row((int32_t)cls);
    if (e.R16s(row + 0x96) != 0x19) {
        e.H(0xbf80, 0x6fb8359f, e.H(0xbf92, 0x1861), 0x1861);
        e.H(0x3664, 0xffffffffu);
        return;
    }
    uint32_t a = ab[0], b = ab[1];
    uint32_t v1[2] = { b * (uint32_t)-25, a * (uint32_t)-25 };   // [esp+0x18] = y, [esp+0x1c] = x
    uint32_t s1[2] = { v1[0], v1[1] };                           // the capture logs the pair as the callee receives it
    e.H(0xc2f8, (uint32_t)(uintptr_t)&v1[1], (uint32_t)(uintptr_t)&v1[0]);
    if (e.blk) e.blk(e.ctx, 0xc2f8, (uint32_t)(uintptr_t)s1, 2);
    uint32_t v2[2] = { b, a };                                   // [esp+0x20] = y, [esp+0x24] = x
    uint32_t s2[2] = { v2[0], v2[1] };
    e.H(0xc2f8, (uint32_t)(uintptr_t)&v2[1], (uint32_t)(uintptr_t)&v2[0]);
    if (e.blk) e.blk(e.ctx, 0xc2f8, (uint32_t)(uintptr_t)s2, 2);
    uint32_t pa = e.R(m + 0x54);
    if (pa) { uint32_t mo = e.R(pa + 0x30); if (mo) { e.W(mo + 4, v1[1] << 11); e.W(mo + 8, v1[0] << 11); e.W(mo + 0xc, 0xc8000); } }
    pa = e.R(m + 0x54);
    if (pa) { uint32_t mo = e.R(pa + 0x30); if (mo) { e.W(mo + 0x10, v2[1] << 11); e.W(mo + 0x14, v2[0] << 11); e.W(mo + 0x18, 0xffff8000u); } }
    e.H(0xc7fb0, m);
}
