// 1.13c D2Client+0x43550 -- EAX = value. When [11C3D0], [11C388] and [11C3B0] are all 0: [119938] = value (9 when value >= 0x1D, signed),
// BE350(1, 0) (stdcall, via the host seam), [11C3D0] = 600, [11C3A4] = 1, [11C3A0] = 0, [11C39C] = 0, [F8C9C] = 0.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
struct D2c43550G { int32_t g11c3d0, g11c388, g11c3b0, g119938, g11c3a4, g11c3a0, g11c39c, gf8c9c; };
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_43550(D2cHost h, int32_t eax, D2c43550G* g) {
    if (g->g11c3d0 || g->g11c388 || g->g11c3b0) return;
    g->g119938 = eax < 0x1D ? eax : 9;
    D2cCall c = {}; c.rva = 0xBE350; c.s[0] = 1; c.s[1] = 0; h(&c);
    g->g11c3d0 = 600; g->g11c3a4 = 1; g->g11c3a0 = 0; g->g11c39c = 0; g->gf8c9c = 0;
}
