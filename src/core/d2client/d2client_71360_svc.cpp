// 1.13c D2Client+0x71360: release the three cached handles (DAT 0x11C000, 0x11C008, 0x11C004). Each nonzero one goes through
// thunk 0xC18A (stdcall 1), then thunk 0xBF86 (ECX = handle, EDX = module string 0xD6AB0, stack 0x7A, 0), then is cleared. Tail jump to 0xBE020.
#include "d2client_callee.h"
extern "C" void d2c_71360_svc(const D2cSvc& v, uint32_t* h /*[0]=0x11C000 [1]=0x11C008 [2]=0x11C004*/) {
    for (int i = 0; i < 3; ++i) {
        const uint32_t x = h[i];
        if (!x) continue;
        d2c_call(v, 0xc18a, 0, 0, 0, 0, 0, x);
        d2c_call(v, 0xbf86, 0, x, 0xd6ab0, 0, 0, 0x7a, 0);
        h[i] = 0;
    }
    d2c_call(v, 0xbe020, 0, 0, 0, 0, 0);
}
