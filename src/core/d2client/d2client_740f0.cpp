// 1.13c D2Client+0x740F0 -- start a skill description's cast: plays the description's two effect words, then either switches
// an existing state handle (mode 8, unit+0x48 = 0x500, 0xCB62) or creates one (0xC442 .. 0xC30A). ECX = unit, EDX = skill
// description index, one unused stack word (ret 4). Returns 0 / 1.
#include "cl_env.h"
extern "C" uint32_t d2c_740f0(uint32_t unit, int32_t idx, const cl::Env& e) {
    if (idx < 0) return 0;
    const uint32_t dt = e.dt;
    if (idx >= (int32_t)e.R(dt + 0xba0)) return 0;
    const uint32_t desc = (uint32_t)idx * 0x23c + e.R(dt + 0xb98);
    if (!desc) return 0;
    if (!e.H(0xc1fc, unit, 0xc)) {
        const int32_t w = e.R16s(desc + 0x10c);
        if (w > 0 && w < (int32_t)e.R(dt + 0xbc0)) e.H(0x6b300, unit, 2, 0, 0, 0, (uint32_t)w);
        const int32_t si = e.R16s(desc + 0x10e);
        if (si > 0) e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, (uint32_t)si);
    }
    const uint32_t p = e.H(0xc1c0, unit, 0xc);
    if (p) {
        e.H(0xcb56, unit, 8);
        e.W(unit + 0x48, 0x500);
        e.H(0xcb62, p, 7);
        return 1;
    }
    uint32_t type = 6, id = 0xffffffffu;
    if (unit) { id = e.R(unit + 0xc); type = e.R(unit); }
    const uint32_t s = e.H(0xc442, 0, 2, 0x15, type, id);
    if (!s) return 0;
    e.H(0xc36a, unit, s, 1);
    e.H(0xc2bc, s, 0x6FB23B50u);
    e.H(0xc2f2, s, 0xc);
    e.H(0xc30a, unit, 0xc, 1);
    return 1;
}
