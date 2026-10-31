// 1.13c D2CMP+0x1BA0 (stdcall p): AUCELLIST constructor -- clear +4..+0x10, an empty list at +0x2c
// (offset 0, link {self, ~self} at +0x30).
#include "d2cmp_guest.h"
using namespace d2cmpg;
// 1.13c D2CMP+0x1BA0
extern "C" uint32_t d2cmp_1ba0(uint8_t* m, uint32_t p) {
    wr(m, p + 4, 0); wr(m, p + 8, 0); wr(m, p + 0xc, 0); wr(m, p + 0x10, 0);
    wr(m, p + 0x30, p + 0x30);
    wr(m, p + 0x2c, 0);
    wr(m, p + 0x34, ~(p + 0x30));
    return p;
}
