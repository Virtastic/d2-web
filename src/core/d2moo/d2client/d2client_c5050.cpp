// 1.13c D2Client+0xC5050: fastcall(ECX = unit, EDX = skill index) + 1 unread stack word (ret 4).
// Skill row (sgptDataTables+0xB98, stride 0x23C, count +0xBA0) missing returns 0. Row word +0xEC = missile index; outside [0, count at +0xB6C) returns 1.
// s = #A1540(EAX = unit, stack missile, skill, 1) == 0 returns 0. m = #A5F70(); if m: #C7B00(EAX = s), #6A6B0(EAX = m, stack 0xE, &B, &A) fills B and A,
// #C7E30(EAX = s, ECX = -A, EDX = 0, stack B). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_c5050(uint32_t unit, int32_t idx, const cl::Env& e) {
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)idx * 0x23c;
    if (!row) return 0;
    int32_t mis = e.R16s(row + 0xec);
    if (mis < 0 || mis >= (int32_t)e.R(e.dt + 0xb6c)) return 1;
    uint32_t s = e.H(0xa1540, (uint32_t)mis, (uint32_t)idx, 1, 0, 0, unit);
    if (!s) return 0;
    uint32_t m = e.H(0xa5f70);
    if (m) {
        uint32_t loc[2] = {};   // [0] = A (esp+8), [1] = B (esp+0xC)
        e.H(0xc7b00, 0, 0, 0, 0, 0, s);
        e.H(0x6a6b0, 0xe, (uint32_t)(uintptr_t)&loc[1], (uint32_t)(uintptr_t)&loc[0], 0, 0, m);
        e.H(0xc7e30, loc[1], 0, 0, 0, 0, s, 0u - loc[0], 0);
    }
    return 1;
}
