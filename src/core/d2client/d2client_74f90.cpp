// HUD: free the cel list (nodes linked through the unaligned word at +0xCD) and clear its head.
#include "cl_env.h"
// 1.13c D2Client+0x74F90. Each node goes to the pool free with the source-file string and line 0x108.
extern "C" void d2cl_74f90(const cl::Env& e) {
    for (uint32_t n = e.R(0x6FAB0000u + 0x11bfd8); n;) {
        uint32_t next = e.R(n + 0xcd);
        e.H(0xbf86, 0x108, 0, 0, 0, 0, 0, n, 0xB2000000u | 0xd4cb0);
        n = next;
    }
    e.W(0x6FAB0000u + 0x11bfd8, 0);
}
