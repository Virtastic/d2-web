// 1.13c D2Client+0xB02E0 (eax = other unit, esi = unit): re-aim/animate a unit from its table row's two 16-bit fields.
// Callees are D2Common ordinals through the 0xC1xx-0xC4xx thunks, stdcall.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB02E0
extern "C" void d2c_b02e0(uint32_t other, uint32_t u, const cl::Env& e) {
    if (!u) return;
    int32_t idx = (int32_t)e.R(u + 4);
    if (idx < 0) return;
    uint32_t row = e.Row(idx);
    if (!row) return;
    if (other) {
        uint32_t t = e.R(other), m = e.R(other + 0x10);
        if (e.R(other + 0xc4) & 0x10000) other = 0;
        else if (t == 0) { if (m == 0 || m == 0x11) other = 0; }
        else if (t == 1) { if (m == 0 || m == 0xc) other = 0; }
    }
    uint32_t n = e.H(0xc3fa, u);                                                  // Ordinal 11029(unit)
    uint32_t v = (n - 1) * (uint32_t)e.R16s(row + 0x98) + (uint32_t)e.R16s(row + 0x96);
    e.H(0xc4a2, u, v);                                                            // 10558(unit, v)
    e.H(0xc2c8, u, v);                                                            // 10970(unit, v)
    uint32_t p = e.R(u + 0x2c), r;
    if (other) {
        e.H(0xc364, p, other);                                                    // 10336(p, other)
        e.H(0xc448, u, 5);                                                        // 10637(unit, 5)
        r = e.H(0xc190, u, other);                                                // 10522(unit, other)
    } else {
        uint32_t pk = e.H(0xc4ba, u);                                             // 10818(unit) -> two packed s16
        uint32_t ty = e.R(u);
        bool fixed = ty == 2 || (ty > 3 && ty <= 5);
        uint32_t a, b;
        if (fixed) a = e.R(e.R(u + 0x2c) + 0xc);
        else { uint32_t q = e.R(u + 0x2c); a = q ? e.H(0xc3be, q) : 0; }          // 10867(p)
        uint32_t x = (uint32_t)(int32_t)(int16_t)(pk & 0xffff) + a;
        if (fixed) b = e.R(e.R(u + 0x2c) + 0x10);
        else { uint32_t q = e.R(u + 0x2c); b = q ? e.H(0xc2ec, q) : 0; }          // 10750(p)
        uint32_t y = (uint32_t)(int32_t)(int16_t)(pk >> 16) + b;
        e.H(0xc3ee, p, x, y);                                                     // 10286(p, x, y)
        e.H(0xc448, u, 6);
        r = e.H(0xc4d8, u, x, y);                                                 // 10080(unit, x, y)
    }
    if ((int32_t)r < 0x19) e.H(0xc376, p, u, 0);                                  // 10334(p, unit, 0)
}
