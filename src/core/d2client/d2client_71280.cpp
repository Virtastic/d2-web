// 1.13c D2Client+0x71280: two cel draws, stdcall(sel). Record +0x34 = DAT 0x11C000 (sel != 0) or 0x11C008; record +0 = 0 then 1.
#include "d2client_callee.h"
extern "C" void d2c_71280_svc(const D2cSvc& v, int32_t sel, uint32_t colA /*0x11C000*/, uint32_t colB /*0x11C008*/, int32_t px, int32_t py) {
    uint32_t blk[18] = {};
    blk[13] = sel ? colA : colB;
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, (uint32_t)(px + 0xbc), (uint32_t)(0x148 - py), 0xffffffffu, 5, 0);
    blk[0] = 1;
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, (uint32_t)(px + 0x1bc), (uint32_t)(0x148 - py), 0xffffffffu, 5, 0);
}
