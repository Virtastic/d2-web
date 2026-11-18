// 1.13c D2Client+0x29250: panel draw. EBX = (D2Gfx #10078-class query 0xD10E == 3) rides through the ten panel
// draws; when 0xD0AE reports 2 the HUD-bar halves 0x271C0 (global 0x11C414 is 2 or 3) and 0x270D0 (1 or 3) draw
// first. Then, for the player at 0x11BBFC with a D2Common #10331 object, a #10826 value differing from 0x1234B0 is
// stored there and 0x1234AC := 60. Tail: 0x267B0.
#include "d2client_callee.h"
// g: [0] 0x11C414, [1] 0x11BBFC, [2] 0x1234B0, [3] 0x1234AC
// 1.13c D2Client+0x29250
extern "C" void d2c_29250(const D2cSvc& v, uint32_t* g) {
    const uint32_t b = d2c_call(v, 0xd10e, 0, 0, 0, 0, 0) == 3 ? 1u : 0u;
    if (d2c_call(v, 0xd0ae, 0, 0, 0, b, 0) == 2) {
        if (g[0] == 2 || g[0] == 3) d2c_call(v, 0x271c0, 0, 0, 0, b, 0);
        if (g[0] == 1 || g[0] == 3) d2c_call(v, 0x270d0, 0, 0, 0, b, 0);
    }
    static const uint32_t seq[10] = { 0x27290, 0x27bd0, 0x27a90, 0x26970, 0x28320, 0x278b0, 0x26880, 0x28dc0, 0x28ce0, 0x289a0 };
    for (uint32_t r : seq) d2c_call(v, r, 0, 0, 0, b, 0);
    if (g[1]) {
        const uint32_t o = d2c_call(v, 0xc1f0, 0, 0, 0, 0, 0, g[1]);
        if (o) {
            const uint32_t n = d2c_call(v, 0xc1de, 0, 0, 0, 0, 0, o);
            if (g[2] != n) { g[2] = n; g[3] = 0x3c; }
        }
    }
    d2c_call(v, 0x267b0, 0, 0, 0, 0, 0);
}
