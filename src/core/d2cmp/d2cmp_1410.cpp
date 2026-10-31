// 1.13c D2CMP+0x1410 (ECX = this): 0x1880(this, 1); zero +0x10 and +0x24; destroy the bucket
// array at +0x14 (0x1B10).
#include "d2cmp_guest.h"
using namespace d2cmpg;
extern "C" void d2cmp_1b10(uint8_t* m, uint32_t arr, const D2CmpGuestEnv* e);
// 1.13c D2CMP+0x1410
extern "C" void d2cmp_1410(uint8_t* m, uint32_t self, const D2CmpGuestEnv* e) {
    e->f1880(m, self, 1);
    wr(m, self + 0x10, 0);
    wr(m, self + 0x24, 0);
    d2cmp_1b10(m, self + 0x14, e);
}
