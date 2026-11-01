// 1.13c D2Client+0x1FFC0 -- monster death-effect spawn: fastcall ECX = unit, one unused stack word (ret 4). The monstats2-shaped
// row (sgptDataTables +0xA78, stride 0x1A8, count +0xA80) gives a short at +0x56 + 2 * difficulty (gl+0 = byte D2Client+0x11C390);
// 10569(unit, &kind, &other); a live unit (+0x10 == 0) whose kind is 4 spawns 0x1AB (short 1) or 0x1AC (short >= 2) at its
// position, then always 0x1AA, through 0xB0450 (ESI = unit; stack id, x, y, 0, 1).
#include "cl_env.h"
extern "C" void d2c_1ffc0(uint32_t u, const cl::Env& e) {
    uint32_t row = 0;
    if (u) {
        int32_t id = (int32_t)e.R(u + 4);
        if (id >= 0 && id < (int32_t)e.R(e.dt + 0xa80)) row = e.R(e.dt + 0xa78) + (uint32_t)id * 0x1a8;
    }
    int32_t m = e.R16s(row + 0x56 + e.R8(e.gl) * 2);
    uint32_t k[2] = {0, 0};
    e.H(0xc5ec, u, (uint32_t)(uintptr_t)&k[0], (uint32_t)(uintptr_t)&k[1]);
    if (u && e.R(u + 0x10)) return;
    if (k[0] != 4) return;
    if (m >= 1) {
        uint32_t y = e.PD(u, 0xc2ec, 0x10), x = e.PD(u, 0xc3be, 0xc);
        e.H(0xb0450, m == 1 ? 0x1abu : 0x1acu, x, y, 0, 1, 0, 0, 0, 0, u);
    }
    uint32_t y = e.PD(u, 0xc2ec, 0x10), x = e.PD(u, 0xc3be, 0xc);
    e.H(0xb0450, 0x1aa, x, y, 0, 1, 0, 0, 0, 0, u);
}
