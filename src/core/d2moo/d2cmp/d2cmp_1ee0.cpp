// 1.13c D2CMP+0x1EE0 (EDX = table, ESI = key): the node holding key in its bucket, or 0.
#include "d2cmp_guest.h"
using namespace d2cmpg;
// 1.13c D2CMP+0x1EE0
extern "C" uint32_t d2cmp_1ee0(uint8_t* m, uint32_t t, uint32_t key) {
    uint32_t h = rd(m, rd(m, t + 0x1c) + (key & rd(m, t + 0x24)) * 0xc + 8);
    uint32_t n = ((int32_t)h < 1) ? 0 : h;
    for (;;) {
        if ((int32_t)n < 1) return 0;
        if (rd(m, n) == key) return n;
        n = rd(m, n + rd(m, rd(m, t + 0x1c) + (key & rd(m, t + 0x24)) * 0xc) + 4);
    }
}
