// 1.13c D2Client+0x289A0: hovered-item nameplate. Every callee goes through the injected dispatcher;
// the Unicode imports are keyed by their IAT slot RVA (0xCEB00 ctor, 0xCEAB8 strcpy, 0xCEABC strcat). The EDI scratch buffer that 0x60B0 and 0x6F5F0
// take is the dispatcher's own: the D2cSvc call has no EDI slot, so the port does not pass it.
#include "d2client_callee.h"
// 1.13c D2Client+0x289A0
extern "C" void d2c_289a0(const D2cSvc& v, uint32_t unit /*[0x11BBFC]*/, uint32_t item /*[0x11C47C]*/, uint32_t enable /*0x11C468*/, uint32_t g46c /*0x11C46C*/,
                          uint32_t g470 /*0x11C470*/, int32_t g538c /*0xF538C*/, int32_t hx /*0xF5390*/, int32_t hy /*0xF5394*/) {
    uint16_t A[256], B[128], C[384], D[64];
    A[0] = 0; for (int i = 1; i < 256; ++i) d2c_call(v, 0xceb00, 0, (uint32_t)(uintptr_t)&A[i], 0, 0, 0);   // Unicode ctor
    B[0] = 0; for (int i = 1; i < 128; ++i) d2c_call(v, 0xceb00, 0, (uint32_t)(uintptr_t)&B[i], 0, 0, 0);
    C[0] = 0; for (int i = 1; i < 384; ++i) d2c_call(v, 0xceb00, 0, (uint32_t)(uintptr_t)&C[i], 0, 0, 0);
    if (!enable || !item) return;
    if (d2c_call(v, 0xc202, 0, 0, 0, 0, 0, *(const uint32_t*)(uintptr_t)(unit + 0x60))) return;
    if (g538c > 3 && !g46c && !g470) return;
    if (!d2c_call(v, 0xc1ba, 0, 0, 0, 0, 0, *(const uint32_t*)(uintptr_t)(item + 4))) {
        const uint32_t msg = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, 0x61d);
        d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xB2000000u | 0xd359f, msg);
        d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);   // does not return in retail
        return;
    }
    d2c_call(v, 0x914f0, 0, 0, 0, 0, 0, item, (uint32_t)(uintptr_t)B, 0x80);
    if (d2c_call(v, 0xc19c, 0, 0, 0, 0, 0, item) != 3) d2c_call(v, 0x560b0, 0, 0, 0, 0, 0, item, 1, 0);
    uint32_t t = d2c_call(v, 0xbf5f0, 0, 0, 0, 0, 0, 3);
    d2c_call(v, 0xceab8, 0, (uint32_t)(uintptr_t)C, t, 0, 0);                                    // Unicode::strcpy
    t = d2c_call(v, 0xbf5f0, 0, 0, 0, 0, 0, 0);
    d2c_call(v, 0xceabc, 0, (uint32_t)(uintptr_t)C, t, 0, 0);                                    // Unicode::strcat
    if (hx >= 0 && hy >= 0) {
        if (d2c_call(v, 0x8b760, 0, 0, 0, 0, 0)) {
            D[0] = 0; for (int i = 1; i < 64; ++i) d2c_call(v, 0xceb00, 0, (uint32_t)(uintptr_t)&D[i], 0, 0, 0);
            uint32_t loc = 0;
            if (d2c_call(v, 0x46230, (uint32_t)(uintptr_t)&loc, item, 0, 0, 0, 1, (uint32_t)(uintptr_t)D, 0x40) && D[0]) {
                const uint32_t s = d2c_call(v, 0xd1ec, 0, 0xf9e, 0, 0, 0);
                d2c_call(v, 0xceabc, 0, (uint32_t)(uintptr_t)C, s, 0, 0);
                d2c_call(v, 0xceabc, 0, (uint32_t)(uintptr_t)C, (uint32_t)(uintptr_t)D, 0, 0);
            }
        }
        d2c_call(v, 0xd384, 0, (uint32_t)(uintptr_t)C, (uint32_t)hx, 0, 0, (uint32_t)hy, 0, 1);
    }
}
