// 1.13c D2Client+0xAF7F0: server packet {?, type, id dword at +2, row index word at +6}: the unit comes from 0xA4E20 (EAX = id & 0x7F, ECX = id,
// EDX = the per-type hash table 0x10A608 + type * 0x200); none returns 0. The row index is looked up in the table behind *sgptDataTables
// (rows +0xB98, count +0xBA0, stride 0x23C); past the count the slot pointer itself is returned (EAX is left holding it). The row's word at +0x2E,
// minus 0x43, selects a prelude: 0 runs 0x2B4C0 (EDI = unit), 9 runs 0xC6450 (EDI = unit), 10 and 11 run 0xC7290 (EAX = unit); everything else
// goes straight on. Then 0xC30A(unit, 0x12, 0) is the result.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_af7f0(uint32_t pk, uint32_t slot, const Env& e, const GEnv& g) {
    const uint32_t type = B(pk + 1), id = e.R(pk + 2), idx = W16(pk + 6);
    const uint32_t u = e.H(0xa4e20, type, 0, 0, 0, 0, id & 0x7f, id, g.M(0x10a608 + type * 0x200));
    if (!u) return 0;
    if (idx >= e.R(e.dt + 0xba0)) return slot;
    const uint32_t row = e.R(e.dt + 0xb98) + idx * 0x23c;
    if (!row) return 0;
    const uint32_t sel = (uint32_t)(e.R16s(row + 0x2e) - 0x43);
    if (sel == 0) { e.H(0x2b4c0); e.Edj(0x2b4c0, u); }
    else if (sel == 9) { e.H(0xc6450); e.Edj(0xc6450, u); }
    else if (sel == 10 || sel == 11) e.H(0xc7290, 0, 0, 0, 0, 0, u);
    return e.H(0xc30a, u, 0x12, 0);
}
