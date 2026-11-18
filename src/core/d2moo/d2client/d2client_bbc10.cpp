// 1.13c D2Client+0xbbc10 -- missile callback: skill-sized child burst. ECX = unit, no return.
// Invalid Missiles row (or s16 +0x1e < 0): 0xB1720(ESI = u). a = s16 +0x1e, raised by 0x1380(ECX = u+0x20, ESI = b-a+1)
// when a < b = s16 +0x20. cls = 10240(u) indexes the 0x23C-byte table at [dt+0xB98] (count [dt+0xBA0]); out of range: 0xB1720.
// p = 0xA60D0(EAX = u), m = 11029(u); r1 = 10786(p, q+0x138, cls, m), r2 = 10786(p, q+0x13C, cls, m). r2 == 0 or r1 <= 0:
// 0xB1720. Else v = 0xB8DF0(EAX = r2, EBX = u; r1, a), a non-zero v goes to 0xB2E00(EAX = v), tail 0xB17C0(ECX = u).
#include <cstdint>
#include "cl_env.h"
extern "C" void d2c_bbc10(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t a = e.R16s(row + 0x1e), b = e.R16s(row + 0x20);
    if (a < b) a += (int32_t)e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, (uint32_t)(b - a + 1));
    int32_t cls = (int32_t)e.H(0xc388, u);
    uint32_t q = 0;
    if (cls >= 0 && cls < (int32_t)e.R(e.dt + 0xba0)) q = e.R(e.dt + 0xb98) + (uint32_t)cls * 0x23c;
    if (!q) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    uint32_t m = e.H(0xc3fa, u);
    int32_t r1 = (int32_t)e.H(0xc316, p, e.R(q + 0x138), (uint32_t)cls, m);
    uint32_t r2 = e.H(0xc316, p, e.R(q + 0x13c), (uint32_t)cls, m);
    if (!r2 || r1 <= 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t v = e.H(0xb8df0, (uint32_t)r1, (uint32_t)a, 0, 0, 0, r2, 0, 0, u);
    if (v) e.H(0xb2e00, 0, 0, 0, 0, 0, v);
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
