// 1.13c D2Client+0x7C8F0 (244 B, stdcall, ret 4): popup_show. Stack a0 = the D2Lang string id.
// g is the module window 0x113A60..0x113A97: g[0] box (0x113A60), g[1] expiry (0x113A64), g[9] 0x113A84, g[10] 0x113A88,
// g[12] x (0x113A90), g[13] y (0x113A94). Callees go through e.H with the registers retail loads; the Unicode::strlen import is
// a plain count. now = GetTickCount.
#include "cl_env.h"
extern "C" void d2c_7c8f0(uint32_t a0, uint32_t* g, uint32_t now, const cl::Env& e) {
    const uint32_t kFile = 0xB20D4BDCu;   // the retail 0x6FB84BDC, logged module-relative
    if (uint32_t old = g[0]) {
        e.H(0x7b3d0, 0, 0, 0, 0, 0, e.R(old));
        e.H(0xbf86, 0x68a, 0, 0, 0, 0, 0, old, kFile);
        g[0] = 0;
    }
    const uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, a0);
    const uint32_t tok = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 0xd);
    const uint32_t node = e.H(0xbf7a, 0x659, 0, 0, 0, 0, 0, 0x2c, kFile);
    const uint32_t v = e.H(0xd378, 0xb4, 0, 0, 0, 0, 0, s, node + 4);
    e.W(node, v);
    e.H(0x7bf60); e.Ej(0x7bf60, node);   // layout: EDI = node
    g[0] = node;
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, tok);
    if (uint32_t b = g[0]) {
        e.W(b + 0x16, 1);
        uint32_t n = 0;
        while (e.R(s + 2 * n) & 0xffff) ++n;
        g[10] = 1; g[9] = 1;
        g[13] = 100u - ((e.R(b + 0x12) & 0xffff) >> 2);
        g[12] = 0x140u - ((e.R(b + 0x10) & 0xffff) >> 1);
        g[1] = (n + 0x19) * 200u + now;
    }
}
