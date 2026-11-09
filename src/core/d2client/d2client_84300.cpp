// 1.13c D2Client+0x84300 -- item packet "swap into slot". EAX = record (byte +8, dword +0xC, byte +0x10, byte +0x11), [esp+4] = packet
// (byte +3, dword +4, type byte +8, id dword +9, unaligned). The unit is looked up by 0xA4E20(EAX = id & 0x7F, ECX = id, EDX = the
// type's table, type on the stack); a null result ends it, a non-player type (dword +0 != 0) or a null inventory (+0x60) is fatal. 0xC9CA fills a
// stack word that the next 0xC6D0 must return unchanged (else fatal). Afterwards 0xCA3C(rec[0xC], 1) picks 0xA6EF0 + 0x833F0 (EBX = 0; null
// is fatal) or 0x82980 (EAX = the stack word, EBX = 0); the player-only tail runs when the unit is D2Client+0x11BBFC.
#include "cl_env.h"
extern "C" void d2c_84300(uint32_t rec, uint32_t p, const cl::Env& e) {
    const uint32_t ty = e.R8(p + 8), id = e.R(p + 9), pl = e.self;
    const uint32_t esi = e.H(0xa4e20, ty, 0, 0, 0, 0, id & 0x7f, id, 0xB2000000u | (0x10A608u + (ty << 9)));
    if (!esi) return;
    if (e.R(esi) != 0) return;                                 // fatal in retail
    const uint32_t edi = e.R(esi + 0x60);
    if (!edi) return;                                          // fatal in retail
    const uint32_t b11 = e.R8(rec + 0x11);
    uint32_t L = e.H(0xc262, edi, b11);
    e.H(0x693c0, 0, 0, 0, 0, 0, 0, 0, esi);
    e.H(0xca2a, edi, L);
    e.H(0x69a90, 0, 0, 0, 0, 0, e.R8(p + 3), esi);
    e.H(0xc9ca, esi, edi, (uint32_t)(uintptr_t)&L, b11);
    const uint32_t ebp = e.H(0xc6d0, edi, L);
    if (!ebp || ebp != L) return;                              // fatal in retail
    e.H(0xca0c, e.R(esi + 0x60));
    if (!e.H(0xc1a8, ebp, 0x100, 0x96, 0xB2000000u | 0xD4964u)) e.H(0xc544, esi, ebp);
    e.H(0xca4e, esi, ebp, 1, 1);
    e.H(0xcaa8, edi, b11);
    if (e.H(0xca3c, e.R(rec + 0xc), 1)) {
        const uint32_t id4 = e.R(p + 4);
        e.H(0xa6ef0, id4, 4, 0, 0, 0, 0, 0xB2000000u | 0x10AE08u, id4 & 0x7f);
        if (esi != pl) return;
        L = e.H(0x833f0, p, rec, 0, 0, 0, 0, 0, 0, 0, 0);
        if (!L) return;                                        // fatal in retail
    } else {
        e.H(0x82980, 0, e.R8(rec + 8), 0, e.R8(rec + 0x10), 0, L);
    }
    if (esi != pl) return;
    e.H(0x8bbf0);
    e.H(0xc9dc, L, 0);
    e.H(0xc7d8, edi, L);
    const uint32_t r = e.H(0xc202, edi);
    e.H(0x162d0, 0, 0, 0, 0, 0, r);
    e.H(0x81aa0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L);
}
