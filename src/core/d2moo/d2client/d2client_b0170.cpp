// 1.13c D2Client+0xB0170 (206 B, written from the disassembly): ESI = unit, stack a0 a1 a2 (ret 0xC).
// Row = Missiles row of unit[+4] (null unit, negative or >= *(dt+0xB6C) index, or a null row: return; stride 0x1A4, base *(dt+0xB64)).
// D2Common #10098(unit) via 0xC424, #11090(unit, row byte +0x183) via 0xC43C, #11003(0, a0, a1, a2, unit, 1) via 0xC3F4,
// #11164(path, row byte +0x184 ? 0x40 : 0) via 0xC400, #11056(path, table[byte +0x183 * 2 dwords]) via 0xC3AC with path = unit[+0x2C] and the
// table at D2Client+0xDD85C (8-byte stride, the dword at +0; e.gl is its pinned copy). Then unit[+0xC4] = (flags with bit 4 set when
// gdwBitMasks[0x10] & row byte +4, else cleared; then & ~0xA | 0x20), written as two stores (the intermediate value is overwritten).
#include "cl_env.h"
// EAX on return (ret 0xC): 0 for a null unit (entered with EAX 0), the index itself when it is negative or not under the count,
// 0 for a null row, else the flags word with bit 4 settled (the first of the two stores).
extern "C" uint32_t d2c_b0170(const cl::Env& e, uint32_t u, uint32_t a0, uint32_t a1, uint32_t a2) {
    if (!u) return 0;
    const int32_t idx = (int32_t)e.R(u + 4);
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xb6c)) return (uint32_t)idx;
    const uint32_t row = e.R(e.dt + 0xb64) + (uint32_t)idx * 0x1a4;
    if (!row) return 0;
    e.H(0xc424, u);
    e.H(0xc43c, u, e.R(row + 0x183) & 0xff);
    e.Xs(1);
    e.H(0xc3f4, 0, a0, a1, a2, u);
    const uint32_t path = e.R(u + 0x2c);
    e.H(0xc400, path, (e.R(row + 0x184) & 0xff) ? 0x40u : 0u);
    e.H(0xc3ac, path, e.R(e.gl + (e.R(row + 0x183) & 0xff) * 8));
    uint32_t f = e.R(u + 0xc4);
    f = ((e.R(e.bm + 0x10) & e.R(row + 4) & 0xff) != 0) ? (f | 4u) : (f & ~4u);
    e.W(u + 0xc4, f);
    e.W(u + 0xc4, (f & ~0xAu) | 0x20u);
    return f;
}
