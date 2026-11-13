// 1.13c D2Client+0xACAA0: no arguments; module state at D2Client+0x11C388.. (g). State dword +0xC: 7 or 9 with the flags +0x7C, +0x48, +0x0, +0x28 all clear:
// +0x119938 = 8, 0xBE350(1, 0) (ret 8), +0x48 = 0x258, +0x1C = 1, +0x18 = 0, +0x14 = 0, D2Client+0xF8C9C = 0. State (reloaded) 2 or 3 with +0x18 set and +0x48,
// +0x0, +0x28 clear: +0x119938 = 7, the same call, +0x48 = 0x258, +0x18 = 0, +0x14 = 0, +0xF8C9C = 0. Always +0x1C = 1. EAX is the second call's result when
// it ran, else the state.
#include "d2c_seam.h"
extern "C" uint32_t d2c_acaa0(const D2cSeam* s, uint8_t* g /* 0x80 bytes at D2Client+0x11C388 */, uint32_t* w9938, uint32_t* w8c9c) {
    auto rd = [&](uint32_t o) { uint32_t v; std::memcpy(&v, g + o, 4); return v; };
    auto wr = [&](uint32_t o, uint32_t v) { std::memcpy(g + o, &v, 4); };
    uint32_t eax = rd(0xc);
    if ((eax == 7 || eax == 9) && !rd(0x7c) && !rd(0x48) && !rd(0x0) && !rd(0x28)) {
        *w9938 = 8; (*s)(0xbe350, 0, 0, 0, 0, 0, 1, 0);
        eax = rd(0xc); wr(0x48, 0x258); wr(0x1c, 1); wr(0x18, 0); wr(0x14, 0); *w8c9c = 0;
    }
    if ((eax == 2 || eax == 3) && rd(0x18) && !rd(0x48) && !rd(0x0) && !rd(0x28)) {
        *w9938 = 7; eax = (uint32_t)(*s)(0xbe350, 0, 0, 0, 0, 0, 1, 0);
        wr(0x48, 0x258); wr(0x18, 0); wr(0x14, 0); *w8c9c = 0;
    }
    wr(0x1c, 1);
    return eax;
}
