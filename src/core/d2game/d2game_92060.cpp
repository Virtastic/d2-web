// 1.13c D2Game+0x92060: ecx = ctx, edx = unit, two unused stack words (ret 8). 0xE07A0(eax = unit, ecx) runs first; the target is
// 0xA598(unit +0x2C); the answer is 1 when it is neither null nor the unit, neither side has an 0xA808(0xA478(x)) flag, and the target's type (dword at +0) is 0 or 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_92060(uint32_t ecx, uint32_t unit, const D2Seam* e)
{
    e->call(0xE07A0, unit, ecx);
    const uint32_t t = e->call(0xA598, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C));
    if (t == unit || !t) return 0;
    if (e->call(0xA808, 0, 0, 0, 0, 0, e->call(0xA478, 0, 0, 0, 0, 0, unit))) return 0;
    if (e->call(0xA808, 0, 0, 0, 0, 0, e->call(0xA478, 0, 0, 0, 0, 0, t))) return 0;
    const int32_t ty = (int32_t)d2rd32(t);
    return (ty >= 0 && ty <= 1) ? 1 : 0;
}
