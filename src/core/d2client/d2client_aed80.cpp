// 1.13c D2Client+0xAED80: server stat packet {?, id dword at +1, stat byte at +5, value dword at +6}. The unit is found inline in the hash table
// at 0x10A608 (slot (id & 0x7F) * 4, chain link at +0xE4, key at +0xC); a miss returns id. A unit whose type dword is not 0 logs fatal 0x5A and
// goes on. 0xC40C(unit, stat, value, 0) writes, then 0xAEB10 (EAX = stat, ECX = unit, stack value) is the result.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_aed80(uint32_t pk, const Env& e, const GEnv& g) {
    const uint32_t id = e.R(pk + 1), val = e.R(pk + 6), st = B(pk + 5);
    uint32_t n = e.R(g.G(0x10a608) + (id & 0x7f) * 4);
    while (n && e.R(n + 0xc) != id) n = e.R(n + 0xe4);
    if (!n) return id;
    if (e.R(n) != 0) Fatal(e, g, 0x5a);
    e.H(0xc40c, n, st, val, 0);
    return e.H(0xaeb10, val, 0, 0, 0, 0, st, n);
}
