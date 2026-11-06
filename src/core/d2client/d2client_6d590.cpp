// 1.13c D2Client+0x6D590: forwards a unit's +0x2C block to D2Client+0xC3C4. EAX must be nonzero (zero returns), ESI = unit, EBX and EDI are compared (equal returns),
// one stack word arg0 (ret 4). Unit prelude (line 0x53C); type 0 with res 0 or 0x11, or type 1 with res 0 or 0xC, calls 0xC3C4(unit[0x2C], arg0).
#include "d2client_unit_prelude.h"
extern "C" void d2cl_6d590(uint32_t eax, uint32_t unit, uint32_t ebx, uint32_t edi, uint32_t arg0, const cl::Env& e) {   // 1.13c D2Client+0x6D590
    if (!eax) return;
    int32_t type, id, res;
    d2cl_unit_prelude(e, unit, 0xD4E7C, 0x53c, &type, &id, &res);
    if (type == 0) { if (res != 0 && res != 0x11) return; }
    else if (type == 1) { if (res != 0 && res != 0xc) return; }
    else return;
    if (ebx == edi) return;
    e.H(0xc3c4, e.R(unit + 0x2c), arg0);
}
