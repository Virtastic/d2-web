// 1.13c D2CMP+0x1D50 (stdcall, this = self): derived container constructor. Runs the base constructor
// (0x20C0), sets the derived vtable, builds the list head at self+0x28 (sentinel tag 4) and the one at
// self+0x38 (tag 0x10), and stores 0x10 at self+0x34.
#include "d2cmp_guest.h"
using namespace d2cmpg;
extern "C" uint32_t d2cmp_20c0(uint8_t* m, uint32_t self, const D2CmpGuestEnv* e);
// 1.13c D2CMP+0x1D50
extern "C" uint32_t d2cmp_1d50(uint8_t* m, uint32_t self, const D2CmpGuestEnv* e) {
    d2cmp_20c0(m, self, e);
    uint32_t c = self + 0x28, a = c + 4;
    wr(m, self, 0x6fe2fe08u);
    wr(m, a, 0); wr(m, a + 4, 0);
    wr(m, c, 0);
    wr(m, a, a); wr(m, c + 8, ~a);
    wr(m, c, 0xddddddddu);
    wr(m, a, a); wr(m, c + 8, ~a);
    c = self + 0x38; a = c + 4;
    wr(m, a, 0); wr(m, a + 4, 0);
    wr(m, c, 0);
    wr(m, a, a); wr(m, c + 8, ~a);
    wr(m, c, 0x10);
    wr(m, a, a); wr(m, c + 8, ~a);
    wr(m, self + 0x34, 0x10);
    c = self + 0x28;
    if (rd(m, c) != 4) {
        e->f1f30(m, c);
        wr(m, c + 4, c + 4); wr(m, c, 4); wr(m, c + 8, ~(c + 4));
    }
    return self;
}
