// 1.13c D2Client+0xB0F40 -- fastcall, ECX = unit (EDI), EDX = target unit (EBX); the client's re-aim of a missile unit at a target.
// Returns: u == 0 leaves EAX untouched (0 here); a class (u +4) outside [0, count) returns the class; a null Missiles row or target returns the row.
// row +0x5C (signed) > 0: c = 0x1330(EAX = row +0x5C, ECX = target +0x20) - row +0x5C / 2, else 0. When u +0x54 points at a block whose +0x30 motion block
// exists, that is written +4 = 0, +8 = 0, +0xC = c << 11. Then 0xC3C4(u +0x2C, 0xC2D4(target +0x2C)), 0xC3E8(u +0x2C, 0), and
// u +0x44 = 0x1380(ECX = u +0x20, ESI = row byte +0x178) << 8, which is also the return.
#include "cl_env.h"
extern "C" uint32_t d2c_b0f40(uint32_t u, uint32_t t, const cl::Env& e) {
    if (!u) return 0;
    const int32_t idx = (int32_t)e.R(u + 4);
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xb6c)) return (uint32_t)idx;
    const uint32_t row = e.R(e.dt + 0xb64) + (uint32_t)idx * 0x1a4;
    if (!row || !t) return row;
    const int32_t h = (int32_t)e.R(row + 0x5c);
    int32_t c = 0;
    if (h > 0) c = (int32_t)e.H(0x1330, 0, 0, 0, 0, 0, (uint32_t)h, t + 0x20) - (h >> 1);
    const uint32_t pa = e.R(u + 0x54);
    if (pa) {
        const uint32_t mo = e.R(pa + 0x30);
        if (mo) { e.W(mo + 4, 0); e.W(mo + 8, 0); e.W(mo + 0xc, (uint32_t)c << 11); }
    }
    const uint32_t v = e.H(0xc2d4, e.R(t + 0x2c));
    e.H(0xc3c4, e.R(u + 0x2c), v);
    e.H(0xc3e8, e.R(u + 0x2c), 0);
    const uint32_t r = e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, e.R8(row + 0x178)) << 8;
    e.W(u + 0x44, r);
    return r;
}
