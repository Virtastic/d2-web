// 1.13c D2Client+0x6CB30: drops a type-1 (monster) unit's cell and re-runs its class step. ESI = unit (may be 0), no stack args. Unit prelude (line 0x1072);
// type != 1 or id >= dt[0xA80] returns; else cell[res + id*40] and its ready flag (+0x14 dwords) := 0 (e.gl = the cell table, module pointer +0x1154E4), then
// EDI = 1 cleared when D2Client+0xC1F0(unit) is 0, D2Client+0xD0A2() is nonzero or the global +0x11B9B0 (gflag) is nonzero; 0x6C3F0(eax = unit, edi).
#include "d2client_unit_prelude.h"
extern "C" void d2cl_6cb30(uint32_t unit, uint32_t gflag, const cl::Env& e) {   // 1.13c D2Client+0x6CB30
    int32_t type, id, res;
    d2cl_unit_prelude(e, unit, 0xD4FA4, 0x1072, &type, &id, &res);
    if (type != 1) return;
    if (id >= (int32_t)e.R(e.dt + 0xa80)) return;
    const uint32_t cell = e.gl + ((uint32_t)res + (uint32_t)id * 40u) * 4;
    e.W(cell, 0);
    e.W(cell + 0x50, 0);
    uint32_t edi = 1;
    if (!e.H(0xc1f0, unit) || e.H(0xd0a2) || e.R(gflag)) edi = 0;
    e.H(0x6c3f0, edi, 0, 0, 0, 0, unit);
}
