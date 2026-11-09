// 1.13c D2Client+0xbbd40 -- missile callback: delayed child burst. ECX = unit, no return.
// Invalid Missiles row (or s16 +0x1e < 0): 0xB1720(ESI = u). Else a = s16 +0x1e, b = s16 +0x20; when a < b,
// a += 0x1380(ECX = u+0x20, ESI = b-a+1) (seeded random). q = 11029(u) / max(1, row+0x60) (signed);
// v = 0xB8DF0(EAX = max(3, row+0x5c - q), EBX = u; row+0x58 + max(2, q), a); a non-zero v goes to 0xB2E00(EAX = v).
// Tail 0xB17C0(ECX = u).
#include <cstdint>
#include "cl_env.h"
extern "C" void d2c_bbd40(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t a = e.R16s(row + 0x1e), b = e.R16s(row + 0x20);
    if (a < b) a += (int32_t)e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, (uint32_t)(b - a + 1));
    int32_t m = (int32_t)e.H(0xc3fa, u);
    int32_t d = (int32_t)e.R(row + 0x60); if (d < 1) d = 1;
    int32_t q = m / d;
    int32_t n = q < 2 ? 2 : q;
    int32_t c = (int32_t)(e.R(row + 0x5c) - (uint32_t)q); if (c < 3) c = 3;
    uint32_t v = e.H(0xb8df0, e.R(row + 0x58) + (uint32_t)n, (uint32_t)a, 0, 0, 0, (uint32_t)c, 0, 0, u);
    if (v) e.H(0xb2e00, 0, 0, 0, 0, 0, v);
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
