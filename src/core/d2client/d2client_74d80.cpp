// 1.13c D2Client+0x74D80: release the five cel handles at 0x11BFDC..0x11BFEC.
#include "cl_env.h"
extern "C" void d2c_74d80(const cl::Env& e) {
    const uint32_t B = 0x6FAB0000u;
    for (uint32_t a = B + 0x11bfdc; a <= B + 0x11bfec; a += 4) {
        uint32_t h = e.R(a);
        if (!h) continue;
        e.H(0xc18a, h);
        e.H(0xbf86, 0x7a, 0, 0, 0, 0, 0, h, 0xB20D6AB0u);
        e.W(a, 0);
    }
}
