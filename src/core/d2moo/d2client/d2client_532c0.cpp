// 1.13c D2Client+0x532C0 (EAX = unit): clears or sets the one-shot flag 0x123478 from the unit type. Ported from the x86; callees reach
// through the injected dispatcher.
#include "d2client_callee.h"
// 1.13c D2Client+0x532C0
extern "C" void d2c_532c0(const D2cSvc& v, uint32_t unit, uint32_t* d3478 /*0x123478*/, uint32_t cc2ec /*0x11C2EC*/, uint32_t cbbfc /*0x11BBFC*/) {
    if (!unit) return;
    const int32_t k = (int32_t)d2c_call(v, 0xc1de, 0, 0, 0, 0, 0, unit);   // D2Common #10826 unit type (thunk, 1 stack word)
    if (k < 0x83 || k > 0x84) { *d3478 = 0; return; }
    if (*d3478 == 0 && cc2ec && cbbfc) {
        d2c_call(v, 0xa15c0, cbbfc, 0, 0, 0, 0, 0x27f, 0, 1);              // EAX = the local unit (mov esi,eax inside); 3 stack words
        *d3478 = 1;
    }
}
