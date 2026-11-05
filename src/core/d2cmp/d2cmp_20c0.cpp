// 1.13c D2CMP+0x20C0 (stdcall, this = self): base container constructor. Sets the base vtable, two
// intrusive list heads (self+4 sentinel tag 0xC, self+0x14 an array {cap,count,data,granule}), grows the
// array to 4 entries (0x2490, callees injectable) and gives each of the 4 nodes tag 4 with a self-link.
#include "d2cmp_guest.h"
using namespace d2cmpg;
extern "C" void d2cmp_2490(uint8_t* m, uint32_t arr, uint32_t n, const D2CmpGuestEnv* e);
// 1.13c D2CMP+0x20C0
extern "C" uint32_t d2cmp_20c0(uint8_t* m, uint32_t self, const D2CmpGuestEnv* e) {
    uint32_t L = self + 4, a = self + 8;
    wr(m, self, 0x6fe2fe38u);
    wr(m, a, 0); wr(m, a + 4, 0); wr(m, L, 0);
    wr(m, a, a); wr(m, L + 8, ~a);
    wr(m, L, 0xddddddddu);
    wr(m, a, a); wr(m, L + 8, ~a);
    wr(m, self + 0x14, 0); wr(m, self + 0x18, 0); wr(m, self + 0x1c, 0); wr(m, self + 0x20, 0);
    wr(m, self + 0x10, 0);
    if (rd(m, L) != 0xc) {
        e->f1f30(m, L);
        wr(m, a, a); wr(m, L, 0xc); wr(m, L + 8, ~a);
    }
    wr(m, self + 0x24, 3);
    d2cmp_2490(m, self + 0x14, 4, e);
    uint32_t off = 0;
    for (uint32_t i = 0; i <= rd(m, self + 0x24); ++i, off += 0xc) {
        uint32_t n = rd(m, self + 0x1c) + off;
        if (rd(m, n) != 4) {
            e->f1f30(m, n);
            wr(m, n + 4, n + 4); wr(m, n, 4); wr(m, n + 8, ~(n + 4));
        }
    }
    return self;
}
