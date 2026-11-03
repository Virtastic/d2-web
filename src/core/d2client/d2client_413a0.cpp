// 1.13c D2Client+0x413A0 -- fastcall(ECX = unit, EDX = Missiles-table index) + 1 stack word `p` (ret 4): returns 1 or 0.
// Row (sgptDataTables+0xB98, stride 0x23C, count +0xBA0) missing returns 0. A = #40550(EAX = index, unit) < 0 returns 0; C = #C838(ECX = unit, EDX = index)
// == 0 returns 0; n = #40450(p, EDI = index) <= 0 returns 0. Then unit word +0xC4 &= ~0x40 (unit non-null); #A1340(EBX = unit, EDI = &X, stack &Y) == 0 returns 0.
// r = #CB68(C). r > 0: #CB50(C, r - 1). Otherwise a request block (23 dwords, zeroed): [0] = 0x20, [1] = [2] = unit, [4] = A, [7] = X, [8] = Y,
// [11] = index, [12] = p; m = #B30F0(ECX = block); if m: #C7E30(ECX = 0xF, EDX = 0, stack 0); then #CB50(C, n).
// Finally d = #C1FC(unit, 0xC): if d: #CB56(unit, byte +0x16 of the row - 1) and unit word +0x48 = 0x800. Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_413a0(uint32_t unit, int32_t idx, uint32_t p, const cl::Env& e) {
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)idx * 0x23c;
    if (!row) return 0;
    int32_t a = (int32_t)e.H(0x40550, unit, 0, 0, 0, 0, (uint32_t)idx);
    if (a < 0) return 0;
    uint32_t c = e.H(0xc838, 0, 0, 0, 0, 0, 0, unit, (uint32_t)idx);
    if (!c) return 0;
    int32_t n = (int32_t)e.H(0x40450, p);
    e.Ej(0x40450, (uint32_t)idx);
    if (n <= 0) return 0;
    if (unit) e.W(unit + 0xc4, e.R(unit + 0xc4) & 0xffffffbfu);
    uint32_t loc[2] = {};   // [0] = X, [1] = Y
    e.Ed((uint32_t)(uintptr_t)&loc[0]);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&loc[1], 0, 0, 0, 0, 0, 0, 0, unit)) return 0;
    int32_t r = (int32_t)e.H(0xcb68, c);
    if (r > 0) {
        e.H(0xcb50, c, (uint32_t)(r - 1));
    } else {
        uint32_t q[23] = {};
        q[0] = 0x20; q[1] = unit; q[2] = unit; q[4] = (uint32_t)a; q[7] = loc[0]; q[8] = loc[1]; q[11] = (uint32_t)idx; q[12] = p;
        if (e.HB(0xb30f0, (uint32_t)(uintptr_t)q)) e.H(0xc7e30, 0, 0, 0, 0, 0, 0, 0xf, 0);
        e.H(0xcb50, c, (uint32_t)n);
    }
    if (e.H(0xc1fc, unit, 0xc)) {
        e.H(0xcb56, unit, e.R8(row + 0x16) - 1);
        e.W(unit + 0x48, 0x800);
    }
    return 1;
}
