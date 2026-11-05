// 1.13c D2Client+0x8CC50 -- remember a unit whose owner matches the local player's colour (ECX = unit): only a type-0x10B unit; its
// owner a = 0xC1F0(unit), the player's b = 0xC1F0([0x11BBFC]); both non-null and the low bytes of 0xC1EA(0xC1DE(b)) and
// 0xC1EA(0xC1DE(a)) equal: [0xE0EC0] = the unit's id [unit+0xC]. g = pinned globals (+0 playerSelf, +4 the remembered id).
#include "cl_env.h"
extern "C" void d2c_8cc50(uint32_t unit, const cl::Env& e, uint32_t g) {
    if (!unit || e.R(unit + 4) != 0x10b) return;
    uint32_t a = e.H(0xc1f0, unit);
    if (!a) return;
    uint32_t b = e.H(0xc1f0, e.R(g));
    if (!b) return;
    uint8_t cb = (uint8_t)e.H(0xc1ea, e.H(0xc1de, b));
    uint8_t ca = (uint8_t)e.H(0xc1ea, e.H(0xc1de, a));
    if (cb == ca) e.W(g + 4, e.R(unit + 0xc));
}
