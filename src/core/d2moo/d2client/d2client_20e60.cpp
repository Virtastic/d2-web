// 1.13c D2Client+0x20E60 -- unit event callback (ECX = unit, one stack word = the event). After the unit's size
// query, an event of 4 on a unit in mode 0 (or no unit) spawns missile 0x52 at the unit's position and at its four
// diagonal neighbours and plays sound 0x99A on the unit.
#include "cl_pa4.h"
extern "C" uint32_t d2c_20e60(uint32_t u, uint32_t ev, const cl::Env& e) {
    using namespace pa4;
    if (!ev) return 0;
    const uint32_t r0 = e.H(0xc5ec, u, 0xB3000000u, 0xB3000000u);
    if (u) {
        const uint32_t m = e.R(u + 0x10);
        if (m) return m;
    }
    if (ev != 4) return u ? 0 : r0;
    const uint32_t x = ux(e, u), y = uy(e, u);
    spawn(e, u, 0x52, x, y);
    spawn(e, u, 0x52, x + 1, y + 1);
    spawn(e, u, 0x52, x + 1, y - 1);
    spawn(e, u, 0x52, x - 1, y + 1);
    spawn(e, u, 0x52, x - 1, y - 1);
    return e.H(0x88a70, u, 0, 0, 0, 0, 0, 0, 0, 0x99a);
}
