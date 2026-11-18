// 1.13c D2Client+0x6C020: per-class graphics cell step. ESI = entry n, EDI = unit (may be 0), one stack word arg0 (ret 4). The cell table is the module
// pointer global at +0x1154E4 (e.gl = its value, a dword array, 40 dwords per class id, ready flag at +0x14).
// Disassembly-derived: unit prelude (line 0x70A), class id >= dt[0xA80] returns; a nonzero cell returns; ready flag 0: D2Client+0x6AC60
// (eax = unit, words n, arg0) == 1 sets the flag, else clears flag and cell; ready flag set: D2Common #10055 (D2Client+0xC124: type, n, id) == 0 sets the cell.
#include "d2client_unit_prelude.h"
extern "C" void d2cl_6c020(uint32_t n, uint32_t unit, uint32_t arg0, const cl::Env& e) {   // 1.13c D2Client+0x6C020
    int32_t type, id, res;
    d2cl_unit_prelude(e, unit, 0xD4FA4, 0x70a, &type, &id, &res);
    if (id >= (int32_t)e.R(e.dt + 0xa80)) return;
    const uint32_t idx = n + (uint32_t)id * 40u, cell = e.gl + idx * 4, flag = cell + 0x50;
    if (e.R(cell)) return;
    if (e.R(flag) == 0) {
        if (e.H(0x6ac60, n, arg0, 0, 0, 0, unit) == 1) { e.W(flag, 1); return; }
        e.W(flag, 0); e.W(cell, 0);
        return;
    }
    if (e.H(0xc124, (uint32_t)type, n, (uint32_t)id) == 0) e.W(cell, 1);
}
