// 1.13c D2Client+0x53450: spawn the player's level-up style effect object. void, no args. The viewed unit (0x11BBFC) is
// resolved through three imports, an effect object is created at its path coordinates and its sub-block flags are set.
#include <cstdint>
#include "cl_env.h"
namespace { constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; } }
extern "C" void d2c_53450(const cl::Env& e) {
    const uint32_t ebp = e.R(G(0x11BBFC));
    if (!ebp) return;
    const uint32_t u = e.H(0xC1F0, ebp);
    if (!u) return;
    const uint32_t edi = e.H(0xC334, u, 2, 0x98);
    if (!edi) return;
    e.H(0x45570, 0x1252, 0, 0x2D5, 0, 0, 0, 0, 0, 1);                  // EBX = 1, six stack words
    const uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, edi);
    const uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, edi);
    const uint32_t o = e.H(0xAFF10, 0x152, x, y, 0, 1, 0, 0, ebp);      // EDX = viewed unit
    if (o) {
        e.H(0xC7E30, 0, 0, 0, 0, 0, 0, 0x28, 0);
        uint32_t a = e.R(o + 0x54), b = a ? e.R(a + 0x30) : 0;
        if (b) { e.W(b + 0x10, 0); e.W(b + 0x14, 0); e.W(b + 0x18, 0x200); }
        a = e.R(o + 0x54); b = a ? e.R(a + 0x30) : 0;
        if (b) { e.W(b + 0x28, 0); e.W(b + 0x2C, 0); e.W(b + 0x30, 0x46); }
        a = e.R(o + 0x54); b = a ? e.R(a + 0x30) : 0;
        if (b) e.W(b, e.R(b) | 0x20);
    }
    e.H(0x6B300, edi, 2, 0, 0, 0, 0xB5);                                 // EAX = 0xB5, seven stack words
    e.H(0x6A0C0, 1, 0, 0, 0, 0, 0, 0, 0, 0xD3);                          // EBX = 0xD3
}
