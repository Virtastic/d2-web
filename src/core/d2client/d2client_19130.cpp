// 1.13c D2Client+0x19130: EAX = unit being torn down. Clears its slot in the 8-word table at D2Client+0x11B800, releases its owned pointers (+0x2C, +0x60, +0x64) through the import hooks, clears playerSelf (+0x11BBFC) when it is the unit, then tail-jumps to 0x190B0.
// g = host address of the module bytes at RVA 0x11B800 (8 words); gs = the playerSelf cell; gf = the word at RVA 0x11C3B8
#include "cl17.h"
extern "C" uint32_t d2c_190b0(uint32_t edi, const cl::Env& e);
extern "C" uint32_t d2c_19130(uint32_t ebx, uint32_t g, uint32_t gs, uint32_t gf, const cl::Env& e) {
    for (uint32_t i = 0; i < 8; ++i)
        if (e.R(g + i * 4) == ebx) { e.W(g + i * 4, 0); break; }
    e.H(0x69700, 0, 0, 0, 0, 0, ebx);
    if (e.R(gf)) { e.H(0xc47e, ebx); e.H(0xc3ca, ebx); }
    e.H(0xc2ce, 0, e.R(ebx + 0x2c));
    e.W(ebx + 0x2c, 0);
    e.H(0xa5c50, 0);
    e.H(0xca78, e.R(ebx + 0x60));
    e.W(ebx + 0x60, 0);
    uint32_t r = e.H(0xc5c2, ebx, 0);
    if (r) e.H(0xa8630, 0, 0, 0, 0, 0, 0, 0, 0, 0, r);
    uint32_t s = e.R(ebx + 0x64);
    if (s) { e.H(0xa8630, 0, 0, 0, 0, 0, 0, 0, 0, 0, s); e.W(ebx + 0x64, 0); }
    uint32_t self = e.R(gs);
    if (ebx == self) { if (self) e.H(0xc0250); e.W(gs, 0); self = 0; }
    e.H(0xcfac, 0, ebx);
    cl::Env e2 = e;
    e2.self = self;
    return d2c_190b0(0, e2);
}
