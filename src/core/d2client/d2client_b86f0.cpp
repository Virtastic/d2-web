// 1.13c D2Client+0xb86f0 -- missile callback: first-frame random child. ECX = unit, no return.
// Invalid Missiles row (or s16 +0x1e < 0): 0xB1720(ESI = u). p = 0xA60D0(EAX = u); p == 0 tails into 0xB17C0(ECX = u).
// r = 10209(u) (thunk 0xC322); r != 0: 10254(u, r - 1) (0xC448) and the tail.
// Else the 0x5C-byte request block (zeroed) holds 2, p, p, s16 +0x1e (+0x10), 10240(u) (+0x2C), 11029(u) (+0x30) and the
// jitter dx (+0x1C), dy (+0x20): n = row+0x58, each 0x1330(ECX = u+0x20, EAX = 2n+1) - n; both 0: dx = 0x1330(EAX = n) + 1.
// v = 0xB30F0(ECX = block); v != 0: 10254(v, dx) then 10238(v, dy) (0xC394). Then 10254(u, 0xB0050(EAX = u) - 1) and the tail.
#include <cstdint>
#include "cl_env.h"
extern "C" void d2c_b86f0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (p) {
        uint32_t r = e.H(0xc322, u);
        if (!r) {
            uint32_t q[23] = {};
            q[0] = 2;
            q[4] = (uint32_t)e.R16s(row + 0x1e);
            q[11] = e.H(0xc388, u);
            q[12] = e.H(0xc3fa, u);
            q[1] = p; q[2] = p;
            uint32_t n = e.R(row + 0x58);
            uint32_t dx = e.H(0x1330, 0, 0, 0, 0, 0, 2 * n + 1, u + 0x20) - n;
            q[7] = dx;
            n = e.R(row + 0x58);
            uint32_t dy = e.H(0x1330, 0, 0, 0, 0, 0, 2 * n + 1, u + 0x20) - n;
            q[8] = dy;
            if (!dx && !dy) { dx = e.H(0x1330, 0, 0, 0, 0, 0, n, u + 0x20) + 1; q[7] = dx; }
            uint32_t v = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            if (v) { e.H(0xc448, v, dx); e.H(0xc394, v, dy); }
            r = e.H(0xb0050, 0, 0, 0, 0, 0, u);
        }
        e.H(0xc448, u, r - 1);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
