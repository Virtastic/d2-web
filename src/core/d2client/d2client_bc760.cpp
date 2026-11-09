// 1.13c D2Client+0xBC760 (400 B, written from the disassembly): EAX = n (clamped to 5..0x5F), stack x, y, ret 8. g = a pinned globals block:
// +0 [0x11B9A0] +4 [0x11B9A4] +8 [0xDBC4C] +0xC [0x119854] +0x10 [0xF218C] +0x14 byte [0x11BBC8] +0x18 playerSelf [0x11BBFC] (Env::self is not used).
// 128 Unicode constructors (0x11070, ECX = buf + 2i) over a 0x80-word local. Only inside x in [g0+0xAD, g0+0x137] and y in [g1+g2-0x11E, g1+g2-0x10B]:
// k = #10212(self, [0x119854]); k <= 0: k = 0xBC430(EAX = self); k > 0: #10431(self, self[0x60], &out, -1, 1), out != 0xD zeroes k.
// row = 0x1230(EAX = [0xF218C]); 0 is the Fog assert (0xBF92(0x15C), 0xBF80, 0x3664(-1), never vectored). name = D2Lang #10003(ECX = word row+6).
// k != 0: fmt = #10003(0x2779), sprintf(0x80, buf, fmt, k, n, name, n); k == 0: fmt = #10003(0x2778), sprintf(0x80, buf, fmt, name, n).
// D2Win #10013(ECX = buf, EDX = g0 + 0x8B; y = g1 + g2 - 0x13B, byte [0x11BBC8], 2, 0).
#include "cl_env.h"
extern "C" void d2c_bc760(const cl::Env& e, uint32_t g, int32_t n, int32_t x, int32_t y) {
    if (n < 5) n = 5; else if (n > 0x5f) n = 0x5f;
    volatile uint16_t buf[128];
    const uint32_t b = (uint32_t)(uintptr_t)&buf[0];
    for (uint32_t i = 0; i < 128; i++) e.H(0x11070, 0, 0, 0, 0, 0, 0, b + 2 * i);
    const int32_t ga = (int32_t)e.R(g), gb = (int32_t)e.R(g + 4), gc = (int32_t)e.R(g + 8);
    if (x < ga + 0xad || x > ga + 0x137) return;
    if (y < gb + gc - 0x11e || y > gb + gc - 0x10b) return;
    const uint32_t self = e.R(g + 0x18);
    int32_t k = (int32_t)e.H(0xc292, self, e.R(g + 0xc));
    if (k < 1) {
        k = (int32_t)e.H(0xbc430, 0, 0, 0, 0, 0, self);
        if (k > 0) {
            volatile uint32_t ov = 0;
            e.H(0xc2aa, self, e.R(self + 0x60), (uint32_t)(uintptr_t)&ov, 0xffffffffu, 1);
            if (ov != 0xd) k = 0;
        }
    }
    const uint32_t row = e.H(0x1230, 0, 0, 0, 0, 0, e.R(g + 0x10));
    if (!row) {
        const uint32_t m = e.H(0xbf92, 0x15c);
        e.H(0xbf80, 0xd359f, m);
        e.H(0x3664, 0xffffffffu);
        return;
    }
    const uint32_t name = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, e.R(row + 4) >> 16);
    if (k) {
        const uint32_t f = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x2779);
        e.H(0x119c0, 0x80, b, f, (uint32_t)k);
    } else {
        const uint32_t f = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x2778);
        e.H(0x119c0, 0x80, b, f, name);
    }
    e.H(0xd3ae, (uint32_t)(gb + gc - 0x13b), e.R(g + 0x14) & 0xff, 2, 0, 0, 0, b, (uint32_t)(ga + 0x8b));
}
