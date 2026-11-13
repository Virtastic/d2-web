// 1.13c D2Client+0xAE230: server packet {?, type, id dword}: the unit comes from 0xA4E20 (EAX = id & 0x7F, ECX = id,
// EDX = the per-type hash table 0x10A608 + type * 0x200, stack type); none returns 0, else 0x84C60 runs with EDI = unit and its result is returned.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_ae230(uint32_t pk, const Env& e, const GEnv& g) {
    const uint32_t type = B(pk + 1), id = e.R(pk + 2);
    const uint32_t u = e.H(0xa4e20, type, 0, 0, 0, 0, id & 0x7f, id, g.M(0x10a608 + type * 0x200));
    if (!u) return 0;
    const uint32_t r = e.H(0x84c60);
    e.Edj(0x84c60, u);
    return r;
}
