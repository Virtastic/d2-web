// 1.13c D2Client+0x68990: allocates and initialises a 0x3C-byte record. The allocation is D2Client+0xBF7A (ECX = 0x3C, EDX = source string, words 0x804, 0); a null
// result takes the fatal path (0xBF92(0x805), 0xBF80(message, code), 0x3664(-1): the process exits, never captured). Else 15 dwords are zeroed, [0x34] = [0x38] = 2.
#include "cl_env.h"
extern "C" uint32_t d2cl_68990(const cl::Env& e) {   // 1.13c D2Client+0x68990
    const uint32_t p = e.H(0xbf7a, 0x804, 0, 0, 0, 0, 0, 0x3c, 0xB2000000u | 0xD4FA4u);
    if (!p) {
        const uint32_t code = e.H(0xbf92, 0x805);
        e.H(0xbf80, 0xB2000000u | 0xD359Fu, code);
        e.H(0x3664, 0xffffffffu);
        return 0;
    }
    for (uint32_t i = 0; i < 15; ++i) e.W(p + 4 * i, 0);
    e.W(p + 0x34, 2); e.W(p + 0x38, 2);
    return p;
}
