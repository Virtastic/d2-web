// 1.13c D2CMP+0x21C0 (stdcall p): empty bucket list -- offset 0xdddddddd (unset), link {p+4, ~(p+4)}.
#include "d2cmp_guest.h"
using namespace d2cmpg;
// 1.13c D2CMP+0x21C0
extern "C" uint32_t d2cmp_21c0(uint8_t* m, uint32_t p) {
    wr(m, p + 4, p + 4);
    wr(m, p + 8, ~(p + 4));
    wr(m, p, 0xddddddddu);
    return p;
}
