// 1.13c D2Client+0x6BF30: the 0x6C020 step over the static cell table at +0x114068 (e.gl = its address; 40 dwords per class id, ready flag at +0x50
// bytes, no class-id bound). ESI = unit (may be 0), EDI = entry n, one stack word arg0 (ret 4). Unit prelude line 0x751.
#include "d2client_unit_prelude.h"
extern "C" void d2cl_6bf30(uint32_t unit, uint32_t n, uint32_t arg0, const cl::Env& e) {   // 1.13c D2Client+0x6BF30
    int32_t type, id, res;
    d2cl_unit_prelude(e, unit, 0xD4FA4, 0x751, &type, &id, &res);
    const uint32_t idx = n + (uint32_t)id * 40u, cell = e.gl + idx * 4, flag = cell + 0x50;
    if (e.R(cell)) return;
    if (e.R(flag) == 0) {
        if (e.H(0x6ac60, n, arg0, 0, 0, 0, unit) == 1) { e.W(flag, 1); return; }
        e.W(flag, 0); e.W(cell, 0);
        return;
    }
    if (e.H(0xc124, (uint32_t)type, n, (uint32_t)id) == 0) e.W(cell, 1);
}
