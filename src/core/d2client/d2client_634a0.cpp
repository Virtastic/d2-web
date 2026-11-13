// 1.13c D2Client+0x634A0 -- load the AutoMapFade setting into a panel (fastcall: ECX = panel; plain ret).
// g = pinned globals, one dword each: +0 [11C1B0] mode, +4 [11C208] fade, +8 [11C20C] override.
// Ordinal 423 reads ("Diablo II", "AutoMapFade", 0, &local); the local starts as the incoming ECX (the push at entry) and the
// callee overwrites it. A value above 4 (unsigned) calls 0x5F970 with EAX = 0. Mode 1 with fade 1 sets fade = (override ? 0 : 2);
// panel +0x124 = fade.
#include "cl_env.h"
extern "C" void d2c_634a0(uint32_t self, const cl::Env& e, uint32_t g) {
    uint32_t local = self;
    e.H(0xbe8a, 0xB2000000u | 0xD3588, 0xB2000000u | 0xD51A4, 0, (uint32_t)(uintptr_t)&local);
    if (local > 4u) e.H(0x5f970, 0, 0, 0, 0, 0, 0);
    if (e.R(g) == 1) {
        uint32_t fade = e.R(g + 4);
        if (fade == 1) { fade = e.R(g + 8) != 0 ? 0u : 2u; e.W(g + 4, fade); }
        e.W(self + 0x124, fade);
        return;
    }
    e.W(self + 0x124, e.R(g + 4));
}
