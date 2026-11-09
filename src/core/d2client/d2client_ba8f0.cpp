// 1.13c D2Client+0xba8f0 -- client missile: hit effect with an optional target-type filter. ECX = unit, EDX = target
// (0 = none); returns 1, or 0 when the target filter rejects. u == 0 or a class outside [0, sgptDataTables +0xB6C)
// returns 1. With a target: its word 0 must be 1 and 0x1510(EAX = target +4, EDX = 0xB) non-zero, else 0.
// Row +0x2C (signed) >= 0: 0xAFF10(EAX = EDX = 0xA60D0(EAX = u); row +0x2C, 0x1630(ECX = u), 0x1660(ECX = u),
// 0xC388(u), 0xC3FA(u)) -- evaluated in retail's push order 0xC3FA, 0xC388, 0x1660, 0x1630, 0xA60D0. Then
// 0xB5B70(ECX = u, EAX = row +0x6C, EDX = row +0x70; row +0x70, row +0x2E, row +0x30 signed) and 1.
#include "cl_env.h"
extern "C" uint32_t d2c_ba8f0(uint32_t u, uint32_t tg, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row) return 1;
    if (tg) {
        if (e.R(tg) != 1) return 0;
        if (!e.H(0x1510, 0, 0, 0, 0, 0, e.R(tg + 4), 0, 0xb)) return 0;
    }
    int32_t m = e.R16s(row + 0x2c);
    if (m >= 0) {
        uint32_t s4 = e.H(0xc3fa, u);
        uint32_t s3 = e.H(0xc388, u);
        uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        e.H(0xaff10, (uint32_t)m, x, y, s3, s4, own, 0, own);
    }
    uint32_t w70 = e.R(row + 0x70);
    e.H(0xb5b70, w70, (uint32_t)e.R16s(row + 0x2e), (uint32_t)e.R16s(row + 0x30), 0, 0, e.R(row + 0x6c), u, w70);
    return 1;
}
