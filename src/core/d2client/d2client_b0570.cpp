// 1.13c D2Client+0xB0570 (ecx = unit, edx = other unit): 1 when edx is 0 or the unit has no 0xA60D0 record; 0 unless that record's type (+0) is 0 and
// the other unit's type is 1; else whether the 0x216A0 lookup of other+0xC equals the record's +0xC. 0x216A0 is inlined here: it walks the node list at
// the global 0x11C4D4 (id +8, value +0xC, next +0x30) for the first node whose id matches and gives its value, or -1 when none does.
#include <cstdint>
#include "cl_genv.h"
using namespace cl;
// 1.13c D2Client+0xB0570
extern "C" uint32_t d2c_b0570(uint32_t u, uint32_t o, const Env& e, const GEnv& g) {
    if (!o) return 1;
    uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!p) return 1;
    if (e.R(p) != 0 || e.R(o) != 1) return 0;
    uint32_t id = e.R(o + 0xc), r = 0xffffffffu;
    for (uint32_t n = e.R(g.G(0x11c4d4)); n; n = e.R(n + 0x30)) if (e.R(n + 8) == id) { r = e.R(n + 0xc); break; }
    return r == e.R(p + 0xc);
}
