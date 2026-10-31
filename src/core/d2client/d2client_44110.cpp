// 1.13c D2Client+0x44110 -- client teardown. Every step is a callee through D2cHost in retail order;
// the function's own state effects are: 11C3B8 := 0, 1154E4 := 0 (when set), the client record's +0xEAA8/+0xEAB0/+0xEA9C/+0x38 := 0, 11C3C4 := 0, 1197E0[32] := 0.
// Module string addresses (the source-file-name strings at D4FA4 / D4BFC / D47DC / D61F8) are passed as 0xB2000000|rva.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_44110(D2cHost h, u32* g3b8, u32* g54e4, u32* g3c4, u32 gbf68, u32 gbf60, u32 gbf64, u32* g97e0 /*32 dwords*/, u32 g3bc) {
    auto call = [&](u32 rva, u32 ecx = 0, u32 edx = 0, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0) { D2cCall c = {}; c.rva = rva; c.ecx = ecx; c.edx = edx; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; h(&c); };
    call(0x80220); call(0x537D0); call(0xA7000);
    if (*g3b8) call(0xCE80, 0, 0, *g3b8);
    *g3b8 = 0;
    call(0xC166); call(0xC16C);
    call(0xC0FA, 0, 0, 0); call(0xC0F4, 0, 0, 0);
    call(0x68C30); call(0x68BE0);
    u32 e54 = *g54e4;
    if (e54) { call(0xC016, 0, e54, 0xB2000000u | 0xD4FA4, 0x7f9, 0); *g54e4 = 0; }
    uint8_t* cl = (uint8_t*)(uintptr_t)*g3c4;
    u32 ea8 = *(u32*)(cl + 0xeaa8);
    if (ea8) call(0xBF86, ea8, 0xB2000000u | 0xD4BFC, 0x55, 0);
    *(u32*)(cl + 0xeaa8) = 0; *(u32*)(cl + 0xeab0) = 0; *(u32*)(cl + 0xea9c) = 0; *(u32*)(cl + 0x38) = 0;
    call(0xBF86, *g3c4, 0xB2000000u | 0xD47DC, 0x141, 0);
    *g3c4 = 0;
    call(0xC2D10); call(0xA82B0);
    call(0xC004, gbf68); call(0xC004, gbf60); call(0xC004, gbf64);
    call(0xC0A0, 0, 0, g97e0[2]);   // 1197E8 = 1197E0 + 8 bytes = dword 2, read before the clear
    for (int i = 0; i < 32; ++i) g97e0[i] = 0;
    call(0xBF86, g3bc, 0xB2000000u | 0xD61F8, 0x592, 0);
}
}
