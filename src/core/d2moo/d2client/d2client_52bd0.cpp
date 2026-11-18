// 1.13c D2Client+0x52BD0 -- overlay request 0x25A at a unit: ESI = unit (null: return). y (0xC2EC) then x (0xC3BE) from its path; r = 0xAFF10(EDX = u;
// 0x25A, x, y, 0, 1); a non-zero r goes to 0xC448(r, 0x19).
#include "cl_env.h"
extern "C" void d2c_52bd0(uint32_t u, const cl::Env& e) {
    if (!u) return;
    uint32_t y = e.PD(u, 0xc2ec, 0x10), x = e.PD(u, 0xc3be, 0xc);
    uint32_t r = e.H(0xaff10, 0x25a, x, y, 0, 1, 0, 0, u);
    if (r) e.H(0xc448, r, 0x19);
}
