// 1.13c D2Client+0xACB70: no arguments. 0x80220(), then 0xBFA4(0) (cdecl), D2Client+0x11C3A0 = 0, +0x11C3A8 = 1, and GetTickCount() into +0x119884.
// EAX is the tick.
#include "d2c_seam.h"
extern "C" uint32_t d2c_acb70(const D2cSeam* s, uint32_t (*get_tick_count)(void), uint32_t* g3a0, uint32_t* g3a8, uint32_t* g9884) {
    (*s)(0x80220);
    (*s)(0xbfa4, 0, 0, 0, 0, 0, 0);
    *g3a0 = 0; *g3a8 = 1;
    return *g9884 = get_tick_count();
}
