// 1.13c D2Game+0x92DF0: ecx = c, edx = unit, stack idx, a1 (ret 8). idx picks a skills row (dt+0xB98, stride 0x23C, count dt+0xBA0);
// 0xAABA(unit) gives the owner record (0 returns 0); 0xE07A0(eax = unit, ecx = c); target = 0xA598(unit +0x2C). A null or self target runs 0xAC94(owner, 0) and returns 0;
// otherwise 0xA754(unit, target, 0) must pass, 0xAC9A(unit, row +0x138, idx, a1) - 1 goes to 0xAC94(owner, n) and 0x92BB0 (ebx = idx; c, unit, target, row, a1, n == 0) runs; returns 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_92df0(uint32_t c, uint32_t unit, uint32_t idx, uint32_t a1, uint32_t dt, const D2Seam* e)
{
    if ((int32_t)idx < 0 || (int32_t)idx >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = idx * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return 0;
    const uint32_t own = e->call(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!own) return 0;
    e->call(0xE07A0, unit, c);
    const uint32_t t = e->call(0xA598, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C));
    if (t == unit || !t) { e->call(0xAC94, 0, 0, 0, 0, 0, own, 0); return 0; }
    if (!e->call(0xA754, 0, 0, 0, 0, 0, unit, t, 0)) return 0;
    const uint32_t n = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x138), idx, a1) - 1;
    e->call(0xAC94, 0, 0, 0, 0, 0, own, n);
    e->call(0x92BB0, 0, 0, 0, idx, 0, c, unit, t, row, a1, n == 0);
    return 1;
}
