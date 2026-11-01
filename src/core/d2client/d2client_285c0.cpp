// 1.13c D2Client+0x285C0: status-panel cel (frame 0) when the expansion check passes and no panel is open.
#include "d2client_callee.h"
// 1.13c D2Client+0x285C0
extern "C" void d2c_285c0(const D2cSvc& v, int32_t w, uint32_t cell /*0x11A78C*/, uint32_t expFlag /*0xFADA8*/, const uint32_t* f /*FADE4 FADB0 FADE0 FAD84 FAD90 FADDC 11C2CC*/) {
    if (!d2c_call(v, 0x9db90, 0, 0, 0, 0, 0)) return;
    const uint32_t blocked = f[0] || f[1] || f[2] || f[3] || f[4];
    if (f[5] && f[6] != 1 && f[6] != 2) return;
    if (blocked) return;
    uint32_t blk[18] = {}; blk[13] = cell;
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, (uint32_t)(w - 0x3a), (expFlag ? 0x46u : 0u) + 0x2d, 0xffffffffu, 1, 0);
}
