// 1.13c D2Client+0x41290 -- fastcall(ECX = owner unit, EDX = Missiles-table index) + 1 stack word `p` (ret 4): spawn one client missile, returns 1 or 0.
// Row = table at sgptDataTables+0xB98 (stride 0x23C, count +0xBA0): out of range or row address 0 returns 0.
// #A1340(EBX = owner, EDI = &X, stack &Y) fills the owner position; 0 returns 0. Row words +0xEA and +0xEC are signed 16-bit: either < 0 returns 0.
// R = #C316(owner, row+0x64, index, p). Request block (23 dwords, zeroed): [0] = 1, [1] = owner, [4] = word +0xEA, [5] = X, [6] = Y,
// [11] = index, [12] = p. m = #B30F0(ECX = block); if m: #CC76(m, word +0xEC) then #CC7C(m, R). Returns 1 whenever the row and position succeed.
#include "cl_env.h"
extern "C" uint32_t d2c_41290(uint32_t owner, int32_t idx, uint32_t p, const cl::Env& e) {
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)idx * 0x23c;
    if (!row) return 0;
    uint32_t loc[3] = {};   // [1] = X, [2] = Y
    uint32_t px = (uint32_t)(uintptr_t)&loc[1], py = (uint32_t)(uintptr_t)&loc[2];
    e.Ed(px);
    if (!e.H(0xa1340, py, 0, 0, 0, 0, 0, 0, 0, owner)) return 0;
    int32_t ea = e.R16s(row + 0xea);
    if (ea < 0 || e.R16s(row + 0xec) < 0) return 0;
    uint32_t r = e.H(0xc316, owner, e.R(row + 0x64), (uint32_t)idx, p);
    uint32_t q[23] = {};
    q[0] = 1; q[1] = owner; q[4] = (uint32_t)ea; q[5] = loc[1]; q[6] = loc[2]; q[11] = (uint32_t)idx; q[12] = p;   // retail stores EBX (the stack word p) here; R only overwrites p's slot for #CC7C
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (m) {
        e.H(0xcc76, m, (uint32_t)e.R16s(row + 0xec));
        e.H(0xcc7c, m, r);
    }
    return 1;
}
