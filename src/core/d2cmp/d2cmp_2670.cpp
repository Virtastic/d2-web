// 1.13c D2CMP+0x2670 (ECX = dst, stdcall src): copy-construct a bucket -- empty link, src's offset.
#include "d2cmp_guest.h"
using namespace d2cmpg;
// 1.13c D2CMP+0x2670
extern "C" uint32_t d2cmp_2670(uint8_t* m, uint32_t dst, uint32_t src) {
    if (dst) {
        wr(m, dst + 4, dst + 4);
        wr(m, dst, rd(m, src));
        wr(m, dst + 8, ~(dst + 4));
    }
    return dst;
}
