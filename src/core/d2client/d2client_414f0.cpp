// 1.13c D2Client+0x414F0 -- fastcall(ECX = unit, EDX = Missiles-table index) + 1 stack word `p` (ret 4): returns 1 or 0.
// Row missing (sgptDataTables+0xB98, stride 0x23C, count +0xBA0) returns 0. A = #40550(EAX = index, unit) < 0 returns 0.
// #A1340(EBX = unit, EDI = &X, stack &Y) == 0 returns 0. n = #40450(p, EDI = index) == 0 returns 0 (a negative n goes on).
// Then #40DF0(EAX = n, EBX = unit, EDI = index; stack unit, X, Y, index, p, A -- ret 0x18); returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_414f0(uint32_t unit, int32_t idx, uint32_t p, const cl::Env& e) {
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    if (!(e.R(e.dt + 0xb98) + (uint32_t)idx * 0x23c)) return 0;
    uint32_t a = e.H(0x40550, unit, 0, 0, 0, 0, (uint32_t)idx);
    if ((int32_t)a < 0) return 0;
    uint32_t loc[2] = {};   // [0] = X, [1] = Y
    e.Ed((uint32_t)(uintptr_t)&loc[0]);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&loc[1], 0, 0, 0, 0, 0, 0, 0, unit)) return 0;
    uint32_t n = e.H(0x40450, p);
    e.Ej(0x40450, (uint32_t)idx);
    if (!n) return 0;
    uint32_t args[6] = { unit, loc[0], loc[1], (uint32_t)idx, p, a };
    e.H(0x40df0, unit, loc[0], loc[1], (uint32_t)idx, p, n, 0, 0, unit);
    e.Ej(0x40df0, (uint32_t)idx);
    e.blk(e.ctx, 0x40df0, (uint32_t)(uintptr_t)args, 6);
    return 1;
}
