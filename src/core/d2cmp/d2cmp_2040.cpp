// 1.13c D2CMP+0x2040 (ECX = table, EAX = bucket index): growth check. Tables at mask >= 0x3ff never
// grow. The pressure counter (+0x10) drops by 3 (floor 0) then rises once per node in the bucket; past
// 0xd it resets and the table doubles (0x2220), returning 1.
#include "d2cmp_guest.h"
using namespace d2cmpg;
extern "C" void d2cmp_2220(uint8_t* m, uint32_t t, uint32_t scratch, const D2CmpGuestEnv* e);
// 1.13c D2CMP+0x2040
extern "C" uint32_t d2cmp_2040(uint8_t* m, uint32_t t, uint32_t idx, const D2CmpGuestEnv* e) {
    if (rd(m, t + 0x24) > 0x3fe) return 0;
    uint32_t c = rd(m, t + 0x10);
    wr(m, t + 0x10, c < 4 ? 0 : c - 3);
    uint32_t n = rd(m, rd(m, t + 0x1c) + idx * 0xc + 8);
    n = ((int32_t)n < 1) ? 0 : n;
    for (;;) {
        if ((int32_t)n < 1) return 0;
        uint32_t v = rd(m, t + 0x10) + 1;
        wr(m, t + 0x10, v);
        if (v > 0xd) break;
        uint32_t b = rd(m, t + 0x1c) + idx * 0xc;
        uint32_t lk = (n == 0) ? b + 4 : rd(m, b) + n;
        n = rd(m, lk + 4);
    }
    wr(m, t + 0x10, 0);
    d2cmp_2220(m, t, e->scratch, e);
    return 1;
}
