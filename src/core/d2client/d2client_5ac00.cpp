// 1.13c D2Client+0x5AC00 -- hireling popup init (no args, plain ret). gl block: +0 the dword 0x11C284, +4 the dword holding the byte 0x11C29C.
// 0x11C284 := 0; 0xC2790(ECX 0x23, EDX 0, stack 0); Fog/Storm #423 (thunk 0xBE8A, 4 stack words) reads registry "Diablo II"/"PopupHireling"
// (D3588, D566C) default 0 into a stack local; a zero local sets the byte 0x11C29C to 1. Returns the local (retail's EAX at the ret).
#include "cl_env.h"
extern "C" uint32_t d2c_5ac00(const cl::Env& e) {
    const uint32_t g = e.gl;
    e.W(g, 0);
    e.H(0xc2790, 0, 0, 0, 0, 0, 0, 0x23, 0);
    uint32_t local = 0;
    e.H(0xbe8a, 0xB2000000u | 0xD3588, 0xB2000000u | 0xD566C, 0, (uint32_t)(uintptr_t)&local);
    if (local == 0) e.W(g + 4, (e.R(g + 4) & 0xffffff00u) | 1);
    return local;
}
