// 1.13c D2Client+0xA3D20, written from the disassembly: pop one level of the window stack. cdecl, no arguments.
// Module globals ride in the pinned array e.gl: +0 depth (0x6FBCBC08), +4 flag (0x6FBAADA8), +8 current window (0x6FBCBC00),
// +12 (0x6FBCBC04), +16 (0x6FBCC2FC), +20 (0x6FB90EA0). Ordinal_10048 (0xD0B4) is cdecl: its string stays on the stack as the
// second word of the 0x3CCC0 / 0x3CC60 calls (EAX = 5 / 0xE / 1). 0xBF86 = Fog free (ECX = ptr, EDX = file, line, 0).
// Module addresses ride as 0xB2000000|rva (the capture's tag).
#include "cl_env.h"
extern "C" void d2cl_a3d20(const cl::Env& e) {
    const uint32_t g = e.gl;
    const uint32_t d = e.R(g) - 1;
    e.W(g, d);
    if (d != 0) { e.H(0xbf80, 0xB20D359Fu, e.H(0xbf92, 0x441)); e.H(0x3664, 0xffffffffu); return; }
    if (e.R(g + 4)) { const uint32_t s = e.H(0xd0b4, 0xB20D0780u); e.H(0x3ccc0, s, 0xB20D0780u, 0, 0, 0, 5); }
    { const uint32_t s = e.H(0xd0b4, 0xB20DE858u); e.H(0x3cc60, s, 0xB20DE858u, 0, 0, 0, 0xe); }
    if (e.H(0x42090) & 0xff) { const uint32_t s = e.H(0xd0b4, 0xB20DE900u); e.H(0x3cc60, s, 0xB20DE900u, 0, 0, 0, 1); }
    const uint32_t cur = e.R(g + 8);
    uint32_t n = e.R(cur + 0xb08);
    e.W(g + 20, 1); e.W(g + 16, 0); e.W(g + 12, 0);
    while (n) { const uint32_t nx = e.R(n + 4); e.H(0xbf86, 0x430, 0, 0, 0, 0, 0, n, 0xB20D41D4u); n = nx; }
    if (e.R(cur + 0xc)) {
        const uint32_t h = e.R(cur + 8);
        if (h) { e.H(0xc18a, h); e.H(0xbf86, 0x7a, 0, 0, 0, 0, 0, h, 0xB20D6AB0u); }
    }
    const uint32_t nxt = e.R(cur + 0xb0c);
    e.H(0xbf86, 0x43b, 0, 0, 0, 0, 0, cur, 0xB20D41D4u);
    e.W(g + 8, nxt);
}
