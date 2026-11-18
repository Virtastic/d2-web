// 1.13c D2CMP+0x1CE0 (EDX = table): next unused key. Bumps the counter at +0x48 (a wrap to 0 sets
// +0x4c and bumps again); once wrapped, skips keys already present in their bucket.
#include "d2cmp_guest.h"
using namespace d2cmpg;
// 1.13c D2CMP+0x1CE0
extern "C" uint32_t d2cmp_1ce0(uint8_t* m, uint32_t t) {
    for (;;) {
        uint32_t id;
        for (;;) {
            id = rd(m, t + 0x48) + 1;
            wr(m, t + 0x48, id);
            if (id != 0) break;
            wr(m, t + 0x4c, 1);
        }
        if (rd(m, t + 0x4c) == 0) break;
        uint32_t h = rd(m, rd(m, t + 0x1c) + (rd(m, t + 0x24) & id) * 0xc + 8);
        uint32_t n = ((int32_t)h < 1) ? 0 : h;
        bool used = false;
        while ((int32_t)n >= 1) {
            if (rd(m, n) == id) { used = true; break; }
            n = rd(m, n + rd(m, rd(m, t + 0x1c) + (rd(m, t + 0x24) & id) * 0xc) + 4);
        }
        if (!used) break;
    }
    return rd(m, t + 0x48);
}
