// 1.13c D2Client+0x73000 -- skill level/charge refill. ECX = unit, EDX = skill (plain ret).
#include "cl_env.h"
extern "C" void d2c_73000(uint32_t unit /*ECX*/, int32_t skill /*EDX*/, const cl::Env& e) {
    if (skill < 0 || skill >= (int32_t)e.R(e.dt + 0xc4)) return;
    uint32_t rec = (uint32_t)skill * 0x3c + e.R(e.dt + 0xbc);
    if (!rec) return;
    uint32_t hs = e.H(0xc1c0, unit, (uint32_t)skill);
    if (!hs) return;
    int32_t v84 = (int32_t)e.H(0xc4b4, hs, 0x84, 0);
    int32_t v85 = (int32_t)e.H(0xc4b4, hs, 0x85, 0);
    int32_t A = e.R16s(rec + 2) < 0 ? -1 : e.R16s(rec + 2);
    int32_t B = e.R16s(rec + 4) < 0 ? -1 : e.R16s(rec + 4);
    int32_t C = e.R16s(rec + 6) < 0 ? -1 : e.R16s(rec + 6);
    int32_t cur = (int32_t)e.H(0xc4b4, hs, 0x163, 0);
    int32_t q = (int32_t)(((int64_t)v85 * 0x55555556) >> 32);
    q += (int32_t)((uint32_t)q >> 31);
    int32_t lvl = v84 >= q ? 3 : 2;
    if (v84 < 2 * q) lvl--;
    e.H(0xc562, hs, 0x163, (uint32_t)lvl, 0);
    if (lvl > cur) {
        e.H(0x729a0, 0, 0, 0, 0, 0, 0, unit, (uint32_t)skill);
        auto two = [&](int32_t v) { e.H(0x6b300, unit, 3, 0, 0, 0, (uint32_t)v); e.H(0x6b300, unit, 3, 0, 0, 0, (uint32_t)v + 1); };
        if (C != -1 && lvl >= 3) two(C);
        if (B != -1 && lvl >= 2) two(B);
        if (A != -1 && lvl >= 1) two(A);
        return;
    }
    auto two2 = [&](int32_t v) { e.H(0x727f0, (uint32_t)v, 0, 0, 0, 0, unit); e.H(0x727f0, (uint32_t)v + 1, 0, 0, 0, 0, unit); };
    if (C != 0 && lvl <= 2) two2(C);
    if (B != 0 && lvl <= 1) two2(B);
    if (A != 0 && A <= 1) two2(A);
}
