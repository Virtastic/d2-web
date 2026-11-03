// 1.13c D2Client+0x40F00 -- client missile ring spawn (the 0x200 variant): EDX = seed init value, EBX = Missiles-table index (hidden registers),
// stack a0..a5 (ret 0x18), no return value.
// A local LCG seed is set up by #C3B2(ECX = &seed, EDX). The request block (23 dwords, zeroed) is: [0] = 3, [1] = a0, [4] = a1, [5] = a2,
// [6] = a3, [11] = EBX, [12] = a4, [21] = 0x6FAF0200 (the do-func at +0x40200). Then for i = 0, a5*4, 2*a5*4 .. while i < 0x100: [7] = word
// at global 0x6FB863E0 + i, [8] = word at 0x6FB862E0 + i (the two direction tables, pinned in e.gl: +0 is 0xD62E0, +0x100 is 0xD63E0),
// one LCG step on the local seed ([22] = the new low word), r = #B30F0(ECX = block); then #C448(r, k + 1) where k is word +0x14C of row EBX
// in the table at sgptDataTables+0xB98 (stride 0x23C, count +0xBA0; 0 when EBX is out of range or the row address is 0). a5 <= 0 never ends.
#include "cl_env.h"
extern "C" void d2c_40f00(uint32_t edx, int32_t ebx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5,
                          const cl::Env& e) {
    uint32_t sd[2] = {};
    uint32_t sp = (uint32_t)(uintptr_t)sd;
    e.H(0xc3b2, 0, 0, 0, 0, 0, 0, sp, edx);
    uint32_t q[23] = {};
    q[0] = 3; q[1] = a0; q[4] = a1; q[5] = a2; q[6] = a3; q[11] = (uint32_t)ebx; q[12] = a4; q[21] = 0x6faf0200u;
    for (uint32_t i = 0; (int32_t)i < 0x100; i += a5 * 4) {
        q[8] = e.R(e.gl + i);
        q[7] = e.R(e.gl + 0x100 + i);
        q[22] = e.Lcg(sp);
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        uint32_t k = 0;
        if (ebx >= 0 && ebx < (int32_t)e.R(e.dt + 0xba0)) {
            uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)ebx * 0x23c;
            if (row) k = e.R(row + 0x14c);
        }
        e.H(0xc448, m, k + 1);
    }
}
