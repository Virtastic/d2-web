// 1.13c D2Client+0x2B530 -- skill start with a client missile: ECX = unit, EDX = skill index, [esp+4] = A, ret 4.
// Rejects an index outside the skills table (0xBA0 count / 0xB98 base, stride 0x23C), a class (row+0xEA, int16) outside 0xB6C, a null 0xA5F70 result or a
// unit without a path (+0x2C). The facing is 0xC610(unit) >> 3 (when 0xC496(path)) or a unit-seed roll 0x1330(8); the heading is
// (roll(3) + ((facing + 4) signed mod 8) - 1) & 7, picking the offsets of the two 8-word tables at module 0xD6A90 (x) and 0xD6A70 (y).
#include "cl_mb2.h"
extern "C" uint32_t d2c_2b530(uint32_t u, int32_t i, uint32_t A, const clm::Env& e) {
    uint32_t row = e.SkillRow(i);
    if (!row) return 0;
    int32_t cls = e.R16s(row + 0xea);
    if (cls < 0 || cls >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    if (u) e.W(u + 0xc4, e.R(u + 0xc4) | 0x40);
    if (!e.H(0xa5f70)) return 0;
    uint32_t path = e.R(u + 0x2c);
    if (!path) return 0;
    uint32_t edi;
    if (e.H(0xc496, 0, 0, 0, 0, 0, path)) edi = (e.H(0xc610, 0, 0, 0, 0, 0, u) & 0xff) >> 3;
    else edi = e.H(0x1330, 8, u + 0x20);
    uint32_t r2 = e.H(0x1330, 3, u + 0x20);
    edi = (edi + 4) & 0x80000007u;
    if ((int32_t)edi < 0) edi = ((edi - 1) | 0xfffffff8u) + 1;
    edi = (r2 + edi - 1) & 7;
    uint32_t x = e.H(0x1630, 0, u) + e.G(0xd6a90 + 4 * edi);
    uint32_t y = e.H(0x1660, 0, u) + e.G(0xd6a70 + 4 * edi);
    int32_t n = (int32_t)e.H(0xc316, 0, 0, 0, 0, 0, u, e.R(row + 0x114), (uint32_t)i, A);
    if (n > 0) cls += (int32_t)e.H(0x1330, (uint32_t)n, u + 0x20);
    e.H(0xb0450, 0, 0, 0, 0, u, (uint32_t)cls, x, y, (uint32_t)i, A);
    return 1;
}
