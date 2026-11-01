// 1.13c D2Client+0x20FB0 -- dispatch on the unit's class id (+4) when its +0x10 word is 0 (0xE for class 0x11C):
// fastcall ECX = unit, one unused stack word (ret 4). Every callee runs with ECX = ESI = unit; the register the
// retail branch loads (EAX, EBX, or EDX for 0x9C) is the unit too.
#include "cl_env.h"
extern "C" void d2c_20fb0(uint32_t u, const cl::Env& e) {
    if (!u) return;
    uint32_t k = e.R(u + 4), f = e.R(u + 0x10);
    enum { A = 1, B = 2, D = 4 };
    uint32_t rva = 0, r = 0, want = 0;
    switch (k) {
    case 0x2d:  rva = 0x52860; r = B; break;
    case 0x9c:  rva = 0x52230; r = D; break;
    case 0xd3:  rva = 0x52620; r = A; break;
    case 0xe5:  rva = 0x527a0; r = A; break;
    case 0xf2:  rva = 0x52270; break;
    case 0xf3:  rva = 0x52600; r = A; break;
    case 0x100: rva = 0x53070; break;
    case 0x10b: rva = 0x52320; break;
    case 0x11c: rva = 0x526e0; r = A; want = 0xe; break;
    case 0x16e: rva = 0x530d0; break;
    case 0x1df: rva = 0x52bd0; break;
    case 0x21c: case 0x21d: case 0x21e: rva = 0x529f0; r = B; break;
    case 0x220: rva = 0x52570; break;
    default: return;
    }
    if (f != want) return;
    e.H(rva, 0, 0, 0, 0, 0, (r & A) ? u : 0, u, (r & D) ? u : 0, (r & B) ? u : 0, u);
}
