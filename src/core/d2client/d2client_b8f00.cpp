// 1.13c D2Client+0xb8f00 -- client missile: skill-levelled twin spawn with a charge counter. ECX = unit, no return.
// Row guard (class against sgptDataTables +0xB6C, row +0x1E >= 0) and skill guard (0xC388(u) against +0xBA0, row
// +0xB98 + id*0x23C) fail into 0xB1720(ESI = u). A = max(row +0x58, 1), B = max(row +0x5C, 1), C = max(row +0x60, 1);
// n = clamp(0xC316(0xA60D0(EAX = u), skill +0x138, id, 0xC3FA(u)), 1, 60), per = 60 / n. With the counter object at
// u +0x64: cur = 0xA8710(EAX = obj), and when (t+1) % per == 0 (t = 0xC4D2(u)) and cur < n, 0xA8B60(EAX = cur+1, ESI = obj).
// At t == 0 the request (word 0 = 1, 1 = owner, 5/6 = 0x1630/0x1660(ECX = u), 4 = row +0x1E, 11/12 = 0xC388/0xC3FA)
// goes to 0xB30F0, and again with word 4 = row +0x20 when that is > 0; each child gets 0xC7E30(EAX = child, ECX = B*A,
// EDX = 0; push -(C*A)) and the path at child +0x54, +0x30 gets +0x10 = C<<11, +0x14 = 0, +0x18 = -B<<11.
// At t == A-2: 0x88C20(u, 1) when 0x22320(EAX = u), and 0x88A70(u, 0, 0, 0; EBX = row +0x34) when that is > 0.
// Every path past the guards ends in 0xB17C0(ECX = u).
#include "cl_env.h"
static void d2c_b8f00_aim(uint32_t m, uint32_t ba, uint32_t ca, uint32_t b, uint32_t c, const cl::Env& e) {
    e.H(0xc7e30, 0u - ca, 0, 0, 0, 0, m, ba, 0);
    uint32_t p = e.R(m + 0x54);
    if (!p) return;
    uint32_t path = e.R(p + 0x30);
    if (!path) return;
    e.W(path + 0x10, c << 11);
    e.W(path + 0x14, 0);
    e.W(path + 0x18, (0u - b) << 11);
}
extern "C" void d2c_b8f00(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t id = (int32_t)e.H(0xc388, u);
    uint32_t sk = 0;
    if (id >= 0 && id < (int32_t)e.R(e.dt + 0xba0)) sk = e.R(e.dt + 0xb98) + (uint32_t)id * 0x23c;
    if (!sk) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t a = (int32_t)e.R(row + 0x58); if (a < 1) a = 1;
    int32_t b = (int32_t)e.R(row + 0x5c); if (b < 1) b = 1;
    int32_t c = (int32_t)e.R(row + 0x60); if (c < 1) c = 1;
    uint32_t ba = (uint32_t)(b * a), ca = (uint32_t)(c * a);
    uint32_t s3 = e.H(0xc3fa, u);
    uint32_t s1 = e.R(sk + 0x138);
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    int32_t n = (int32_t)e.H(0xc316, own, s1, (uint32_t)id, s3);
    if (n <= 1) n = 1; else if (n >= 60) n = 60;
    int32_t per = 60 / n;
    uint32_t obj = e.R(u + 0x64);
    int32_t cur = obj ? (int32_t)e.H(0xa8710, 0, 0, 0, 0, 0, obj) : 0;
    int32_t t = (int32_t)e.H(0xc4d2, u);
    if ((t + 1) % per == 0 && cur < n && obj) e.H(0xa8b60, 0, 0, 0, 0, 0, (uint32_t)(cur + 1), 0, 0, 0, obj);
    if (t == 0) {
        uint32_t q[23] = {};
        q[0] = 1;
        q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        q[4] = (uint32_t)e.R16s(row + 0x1e);
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (m) d2c_b8f00_aim(m, ba, ca, (uint32_t)b, (uint32_t)c, e);
        int32_t w20 = e.R16s(row + 0x20);
        if (w20 > 0) {
            q[4] = (uint32_t)w20;
            m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            if (m) d2c_b8f00_aim(m, ba, ca, (uint32_t)b, (uint32_t)c, e);
        }
    }
    if (t == a - 2) {
        if (e.H(0x22320, 0, 0, 0, 0, 0, u)) e.H(0x88c20, u, 1);
        int32_t w34 = e.R16s(row + 0x34);
        if (w34 > 0) e.H(0x88a70, u, 0, 0, 0, 0, 0, 0, 0, (uint32_t)w34);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
