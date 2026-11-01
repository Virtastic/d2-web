// 1.13c D2Client+0xb6010 -- client missile: a 64-point ring (radius 30) of child missiles, every max(row +0x6C, 1)-th
// point; ECX = missile unit, always returns 1. Child class = Missiles row +0x2C; block word 7 = ring x (0xD3AB8),
// word 8 = ring y (0xD39B8); each child then gets D2Common 0xC448 (x) and 0xC394 (y).
#include "cl_env.h"
namespace {
const int32_t kSin[64] = { 0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29, 30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2,
                           0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29, -30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2 };   // 0xD39B8
}
extern "C" uint32_t d2c_b6010(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    int32_t cid = (int32_t)e.R(u + 4);
    if (cid < 0) return 1;
    uint32_t row = e.Row(cid);
    if (!row) return 1;
    if (e.R16s(row + 0x2c) < 0) return 1;
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 1;
    uint32_t q[23] = {};
    q[0] = 2; q[1] = room; q[2] = u;
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    q[4] = (uint32_t)e.R16s(row + 0x2c);
    int32_t st = (int32_t)e.R(row + 0x6c);
    if (st < 1) st = 1;
    for (uint32_t i = 0; i < 64; i += (uint32_t)st) {
        uint32_t y = (uint32_t)kSin[i], x = (uint32_t)kSin[(i + 16) & 63];   //  = 
        q[8] = y; q[7] = x;
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (m) { e.H(0xc448, m, x); e.H(0xc394, m, y); }
    }
    return 1;
}
