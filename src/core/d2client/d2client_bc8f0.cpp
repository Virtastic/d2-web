// 1.13c D2Client+0xBC8F0 (568 B, written from the disassembly): EAX = x, stack y, n1, n2 (ret 0xC). Two stacked tooltip boxes.
// g = a pinned globals block: +0 [0x11B9A0] +4 [0x11B9A4] +8 [0xDBC4C] +0xC dword [0xF2188] (index) +0x10 byte [0x11BBC8] +0x14 byte [0x11BFF8] (written).
// tab = the table at 0xD35A4, stride 0x18, dwords +0 +4 (box origin), +8 +0xC (title position), +0x10 +0x14 (text position).
// 128 Unicode constructors (0x11070, ECX = buf + 2i), then D2Win #10190(0,0,0), its low byte stored to [0x11BFF8].
// flag1: x in [A+0xA2, A+0x140], y in [B+C-0x14E, B+C-0x13B]; flag2 (only when flag1 clear): same x, y in [B+C-0x134, B+C-0x121].
// n1 == 0 clears flag1, n2 == 0 clears flag2; neither set: return. flag1: idx 0, n = n1; else idx 1, n = n2; n clamped to 5..0x5F.
// D2Gfx #10014(x0, y0, x0+0x9B, y0+0x1E, byte [0x11BBC8], 2), D2Win #10013-style title draw (#10003(0x103f)), then the row of the 0x1A8-stride table at dt+0xA78
// ([0xF2188], range checked against dt+0xA80; else the Fog assert 0xBF92(0xEC), 0xBF80, 0x3664(-1), never vectored):
// sprintf(0x80, buf, #10003(0x2777), #10003(row word +6; ECX keeps the dt pointer high half, from e.gl when the probe pins dt at a module address), n) and a final D2Win #10150 draw.
#include "cl_env.h"
extern "C" void d2c_bc8f0(const cl::Env& e, uint32_t g, uint32_t tab, int32_t x, int32_t y, int32_t n1, int32_t n2) {
    volatile uint16_t buf[128];
    const uint32_t b = (uint32_t)(uintptr_t)&buf[0];
    for (uint32_t i = 0; i < 128; i++) e.H(0x11070, 0, 0, 0, 0, 0, 0, b + 2 * i);
    const uint32_t al = e.H(0xd390, 0, 0, 0) & 0xff;
    e.W(g + 0x14, (e.R(g + 0x14) & ~0xffu) | al);
    const int32_t A = (int32_t)e.R(g), B = (int32_t)e.R(g + 4), C = (int32_t)e.R(g + 8);
    int32_t f1 = 0, f2 = 0;
    if (x >= A + 0xa2 && x <= A + 0x140) {
        if (y >= B + C - 0x14e && y <= B + C - 0x13b) f1 = 1;
        else if (y >= B + C - 0x134 && y <= B + C - 0x121) f2 = 1;
    }
    if (n1 == 0) f1 = 0;
    if (n2 == 0) f2 = 0;
    uint32_t idx = 0;
    int32_t n = n1;
    if (!f1) {
        if (!f2) return;
        idx = 1; n = n2;
    }
    if (n < 5) n = 5; else if (n > 0x5f) n = 0x5f;
    const uint32_t t = tab + idx * 0x18;
    const int32_t x0 = (int32_t)e.R(t) + A, y0 = (int32_t)e.R(t + 4) + B + C - 0x1e0;
    e.Xs(2);
    e.H(0xd0ba, (uint32_t)x0, (uint32_t)y0, (uint32_t)(x0 + 0x9b), (uint32_t)(y0 + 0x1e), e.R(g + 0x10) & 0xff);
    const uint32_t ty = (uint32_t)((int32_t)e.R(t + 0xc) + B + C - 0x1e0);
    const uint32_t tx = (uint32_t)((int32_t)e.R(t + 8) + A);
    const uint32_t s0 = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x103f);
    e.H(0xd37e, ty, 0, 0, 0, 0, 0, s0, tx);
    const int32_t di = (int32_t)e.R(g + 0xc);
    uint32_t row = 0;
    if (di >= 0 && di < (int32_t)e.R(e.dt + 0xa80)) row = e.R(e.dt + 0xa78) + (uint32_t)di * 0x1a8;
    if (!row) {
        const uint32_t m = e.H(0xbf92, 0xec);
        e.H(0xbf80, 0xd359f, m);
        e.H(0x3664, 0xffffffffu);
        return;
    }
    const uint32_t name = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, ((e.gl ? e.gl : e.dt) & 0xffff0000u) | (e.R(row + 4) >> 16));
    const uint32_t f = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x2777);
    e.H(0x119c0, 0x80, b, f, name, (uint32_t)n);
    e.H(0xd37e, (uint32_t)((int32_t)e.R(t + 0x14) + B + C - 0x1e0), 0, 0, 0, 0, 0, b, (uint32_t)((int32_t)e.R(t + 0x10) + A));
}
