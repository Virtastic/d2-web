// 1.13c D2Client+0x54780 -- item string suffix (stdcall dest, ret 4). Looks up D2Lang strings 0xFA2, 0xF9E, 0xF9B, 0xFA1 (in that order),
// formats 0x32 as "%d", then builds in a 128-char local: str(0xFA2) "50" str(0xFA1) str(0xF9B) str(0xDE2) str(0xF9E), and appends it to dest.
#include "d2c_wstr.h"
struct D2c54780Env { uint32_t (*str)(uint32_t id); };
extern "C" void d2c_54780r(uint32_t dst, const D2c54780Env& e) {
    uint16_t loc[128] = {};
    const uint32_t b = e.str(0xfa2), p = e.str(0xf9e), q = e.str(0xf9b), s = e.str(0xfa1);
    uint16_t num[10]; d2c::wdec(num, 0x32);
    const uint32_t L = (uint32_t)(uintptr_t)loc;
    d2c::wcat(L, b); d2c::wcat(L, (uint32_t)(uintptr_t)num); d2c::wcat(L, s); d2c::wcat(L, q);
    d2c::wcat(L, e.str(0xde2)); d2c::wcat(L, p);
    d2c::wcat(dst, L);
}
