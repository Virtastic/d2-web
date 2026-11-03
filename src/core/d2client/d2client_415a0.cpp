// 1.13c D2Client+0x415A0 -- fastcall(ECX = unit, EDX = Missiles-table index) + 1 stack word `p` (ret 4): returns 1 or 0.
// Row missing (sgptDataTables+0xB98, stride 0x23C, count +0xBA0) returns 0. A = #40550(EAX = index, unit) < 0 returns 0. #2A0B0(EAX = unit) runs, then
// the unit's path pointer (+0x2C) must be non-null (retail terminates the process through #BF92/#BF80/#3664(-1) when it is not; no vector reaches it).
// R = #C2DA(path) == 0 returns 0. n = #40450(p, EDI = index); n <= 0: n = #C316(unit, row+0x64, index, p). X = #1630(ECX = R), Y = #1660(ECX = R).
// Then #40F00(EBX = index, EDX = unit word +0x28; stack unit, A, X, Y, p, n -- ret 0x18); returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_415a0(uint32_t unit, int32_t idx, uint32_t p, const cl::Env& e) {
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)idx * 0x23c;
    if (!row) return 0;
    uint32_t a = e.H(0x40550, unit, 0, 0, 0, 0, (uint32_t)idx);
    if ((int32_t)a < 0) return 0;
    e.H(0x2a0b0, 0, 0, 0, 0, 0, unit);
    uint32_t path = e.R(unit + 0x2c);
    if (!path) return 0;
    uint32_t rr = e.H(0xc2da, path);
    if (!rr) return 0;
    uint32_t n = e.H(0x40450, p);
    e.Ej(0x40450, (uint32_t)idx);
    if ((int32_t)n <= 0) n = e.H(0xc316, unit, e.R(row + 0x64), (uint32_t)idx, p);
    uint32_t seed = e.R(unit + 0x28);
    uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, rr);
    uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, rr);
    uint32_t args[6] = { unit, a, x, y, p, n };
    e.H(0x40f00, unit, a, x, y, p, 0, 0, seed, (uint32_t)idx);
    e.blk(e.ctx, 0x40f00, (uint32_t)(uintptr_t)args, 6);
    return 1;
}
