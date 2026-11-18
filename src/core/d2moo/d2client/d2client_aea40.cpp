// 1.13c D2Client+0xAEA40: server unit stat packet, opcodes 0x9E..0xA2 (a jump table over op - 0x9E; larger returns op - 0x9E). Layout {op, type,
// id dword at +2, value from +6}: 0x9E byte, 0x9F word, 0xA0 dword, 0xA1 byte, 0xA2 word; 0xA1 and 0xA2 take the stat write 0xC4F6, the
// others 0xC40C (the jump table enters 0x9F at the word load, past the flag set). The unit is found inline in the hash table at 0x10A808 (slot (id & 0x7F) * 4, chain link at +0xE4, key at +0xC); a miss returns
// id & 0x7F. A unit whose type dword is not 1 logs fatal 0x5A and goes on. Type 0xC also runs 0xAE3F0(unit). Returns the stat write's result.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_aea40(uint32_t pk, const Env& e, const GEnv& g) {
    const uint32_t op = B(pk), sel = op - 0x9e;
    if (sel > 4) return sel;
    uint32_t val, wide;
    switch (sel) {
    case 0: val = B(pk + 6); wide = 0; break;
    case 1: val = W16(pk + 6); wide = 0; break;
    case 2: val = e.R(pk + 6); wide = 0; break;
    case 3: val = B(pk + 6); wide = 1; break;
    default: val = W16(pk + 6); wide = 1; break;
    }
    const uint32_t type = B(pk + 1), id = e.R(pk + 2);
    uint32_t n = e.R(g.G(0x10a808) + (id & 0x7f) * 4);
    while (n && e.R(n + 0xc) != id) n = e.R(n + 0xe4);
    if (!n) return id & 0x7f;
    if (e.R(n) != 1) Fatal(e, g, 0x5a);
    if (type == 0xc) e.H(0xae3f0, 0, 0, 0, 0, 0, n);
    return e.H(wide ? 0xc4f6 : 0xc40c, n, type, val, 0);
}
