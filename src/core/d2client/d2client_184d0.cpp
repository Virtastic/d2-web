// 1.13c D2Client+0x184D0: s0 = struct, s1 = unit id (+0x60 = inventory); d2common type 4/5 pair test -> 6 in AL, else the 0xCFA6 lookup (ret 8)
#include "cl17.h"
extern "C" uint32_t d2c_184d0(uint32_t s0, uint32_t s1, const cl::Env& e) {
    uint32_t t = e.H(0xca54, s1) & 0xff;
    if (t == 5 || t == 4) {
        uint32_t esi = e.R(s0 + 0x60);
        uint32_t edi = e.H(0xc262, esi, 4);
        uint32_t ebx = e.H(0xc262, esi, 5);
        if (edi && ebx && !e.H(0xc87a, edi) && !e.H(0xc87a, ebx)) {
            uint32_t eax = e.H(0xca0c, esi);
            if (s1 != eax) return (eax & 0xffffff00u) | 6;
        }
    }
    return e.H(0xcfa6, s1);
}
