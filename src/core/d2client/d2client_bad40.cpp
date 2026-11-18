// 1.13c D2Client+0xbad40 -- client missile: skill-timed sound/overlay event. ECX = unit; returns 1, or 0 when 0xA4E20
// refuses. u == 0, a class outside [0, sgptDataTables +0xB6C), row +0x24 (signed) < 0 or owner = 0xA60D0(EAX = u) == 0
// return 1. id = 0xC388(u), lvl = 0xC3FA(u); skill row (id against +0xBA0, +0xB98 + id*0x23C) or return 1.
// a = 0xC322(u), t = 0xC4BA(u); 0xA4E20(EAX = t & 0x7F, ECX = t, EDX = D2Client+0x10A608 + a*0x200; a) == 0 returns 0.
// x/y = 0x1630/0x1660(ECX = u). n = row +0x6C, or when <= 0 max(0xC316(owner, skill +0x64, id, lvl), 1). Row +0x70 <= 0
// calls 0xC316(owner, skill +0x144, id, lvl) (result dropped). f = skill +0x50 ? 0xA683 : 0. 0xA1860(owner, x, y, n, f,
// D2Client+0xB63C0, &{owner, u, id, lvl, ?, row +0x2C}, 0) with EAX = owner, ECX = x, EDX = y.
#include "cl_env.h"
extern "C" uint32_t d2c_bad40(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x24) < 0) return 1;
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!own) return 1;
    uint32_t id = e.H(0xc388, u);
    uint32_t lv = e.H(0xc3fa, u);
    if ((int32_t)id < 0 || (int32_t)id >= (int32_t)e.R(e.dt + 0xba0)) return 1;
    uint32_t sk = e.R(e.dt + 0xb98) + id * 0x23c;
    if (!sk) return 1;
    uint32_t a = e.H(0xc322, u);
    uint32_t t = e.H(0xc4ba, u);
    uint32_t ok = e.H(0xa4e20, a, 0, 0, 0, 0, t & 0x7f, t, 0xB210A608u + (a << 9));   // module address, recorded as 0xB2000000|rva
    if (!ok) return 0;
    uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    uint32_t n = e.R(row + 0x6c);
    if ((int32_t)n <= 0) {
        n = e.H(0xc316, own, e.R(sk + 0x64), id, lv);
        if ((int32_t)n < 1) n = 1;
    }
    if ((int32_t)e.R(row + 0x70) <= 0) e.H(0xc316, own, e.R(sk + 0x144), id, lv);
    uint32_t f = e.R(sk + 0x50) ? 0xa683u : 0u;
    // retail's stack at the call: the eight arguments, the four saved registers (residue, zeroed), then its frame
    // {owner, lvl, row, y, x} and the block {owner, u, id, lvl, uninitialised (zeroed), row +0x2C} the seventh argument points at
    uint32_t k[23] = { own, x, y, n, f, 0xB20B63C0u, 0, 0, 0, 0, 0, 0, own, lv, row, y, x, own, u, id, lv, 0, (uint32_t)e.R16s(row + 0x2c) };
    k[6] = (uint32_t)(uintptr_t)(k + 17);
    e.H(0xa1860, own, x, y, n, f, own, x, y);
    if (e.blk) e.blk(e.ctx, 0xa1860, (uint32_t)(uintptr_t)k, 23);
    return 1;
}
