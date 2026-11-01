// 1.13c D2Client+0x18F90: EAX = unit, EBX = pointer to a cell, s0 = optional out int (ret 4). Only the local player proceeds: reads the mode (0x72080), maybe swaps the cell through 0xC838 when the entry flag matches, then 0x18F40(mode, unit)
#include "cl17.h"
extern "C" uint32_t d2c_18f90(uint32_t unit, uint32_t ebx, uint32_t outp, const cl::Env& e) {
    if (unit != e.self) return 1;
    if (!ebx || !e.R(ebx)) return 0;
    int32_t m = (int32_t)e.H(0x72080);
    if (outp) e.W(outp, (uint32_t)m);
    uint32_t ent = e.H(0xc640, 0, 0, 0, 0, 0, 0, e.R(ebx));
    if (!ent) return 0;
    if (m > 0 && (m < 3 || m == 4) && (e.R8(e.bm + 0x10) & e.R8(ent + 7))) {
        e.W(ebx, e.H(0xc838, 0, 0, 0, 0, 0, 0, unit, 0));
        m = (int32_t)e.H(0x72080);
    }
    if (m == 5 || m == 0) return 1;
    e.H(0x18f40, unit, 0, 0, 0, 0, (uint32_t)m);
    return 0;
}
