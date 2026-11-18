// 1.13c D2Client+0x66C80: draw a panel button and, while the mouse is inside its rectangle, its hover text. __fastcall, ECX = the control,
// no stack arguments. The button cel is loaded once (0x11B9C4) through the buysell path string, the 18-dword frame is drawn through the
// Ordinal 10041 thunk (0xD096), then the cursor (0x11B824 = y, 0x11B828 = x, both u16) is tested against the control's rect.
#include <cstdint>
#include <cstring>
#include "cl_env.h"
namespace { constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; } }
extern "C" void d2c_66c80(uint32_t esi, const cl::Env& e) {
    uint32_t f[18];
    memset(f, 0, sizeof f);
    uint32_t cel = e.R(G(0x11B9C4));
    if (!cel) {
        volatile char path[0x100] = {};
        e.H(0xCF114, (uint32_t)(uintptr_t)path, 0xB2000000u | 0xD0564, 0xB2000000u | 0xD357C);   // wsprintfA(path, fmt, DATA\GLOBAL)
        cel = e.H(0x2B420, 0);
        e.W(G(0x11B9C4), cel);
    }
    f[0] = (e.R(esi + 0x30) & 0xffffu) + e.R(esi + 0x20);
    f[13] = cel;
    const uint32_t fp = (uint32_t)(uintptr_t)f;
    e.H(0xD096, fp, e.R(esi + 8), e.R(esi + 0xC), 0xffffffffu, 5, 0);
    if (e.blk) e.blk(e.ctx, 0xD096, fp, 18);
    const int32_t my = (int32_t)(e.R(G(0x11B824)) & 0xffffu), mx = (int32_t)(e.R(G(0x11B828)) & 0xffffu);
    const int32_t left = (int32_t)e.R(esi + 8), top = (int32_t)e.R(esi + 0xC);
    if (mx < left) return;
    if (mx > (int32_t)e.R(esi + 0x1C) + left) return;
    const int32_t bot = top - (int32_t)e.R(esi + 0x18);
    if (my < bot) return;
    if (my > top) return;
    const uint32_t ebx = e.H(0xD3A8, 0, 0, 0, 0, 0, 0, 1);
    const uint32_t idx = e.R(esi + 0x32);
    const uint32_t id = e.R(G(0xE28F2) + idx * 4) & 0xffffu;
    const uint32_t edi = e.H(0xD1EC, 0, 0, 0, 0, 0, 0, id);
    const int32_t tw = (int32_t)e.H(0xD35A, 0, 0, 0, 0, 0, 0, edi);
    const int32_t w = (int32_t)e.R(esi + 0x1C) - tw;
    const int32_t x = ((w + (w < 0 ? 1 : 0)) >> 1) + left;
    const int32_t y = top - (int32_t)e.R(esi + 0x18) - 5;
    e.H(0xD37E, (uint32_t)y, 0, 0, 0, 0, 0, edi, (uint32_t)x);
    e.H(0xD3A8, 0, 0, 0, 0, 0, 0, ebx);
}
