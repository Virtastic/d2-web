// 1.13c D2Client+0x66270 -- fastcall ECX = this. Reads the "Resolution" setting of section "Diablo II" into this+0x124 (Storm ordinal 423, four stack
// words: section, key, default 0, out pointer), then calls 0x10DF0 with ESI = 2 when the value read is 1, else 0; its answer stays in EAX.
#include "cl_env.h"
extern "C" uint32_t d2c_66270(uint32_t self, const cl::Env& e) {
    const uint32_t p = self + 0x124;
    e.H(0xbe8a, 0x6FB83588u, 0x6FB851DCu, 0, p);
    const uint32_t esi = e.R(p) == 1 ? 2u : 0u;
    return e.H(0x10df0, 0, 0, 0, 0, 0, 0, 0, 0, 0, esi);
}
