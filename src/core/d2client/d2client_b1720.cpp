// 1.13c D2Client+0xB1720 (esi = unit, void): run the unit row's start action (D2Client+0x225D0 / +0x88C20) when
// its +0x12 short is non-negative, notify through ordinal 10398 (then D2Client+0xA85F0), and hand (type, id) to
// D2Client+0xA6EF0 with the 0x200-stride record base picked by bit 21 of +0xC4.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB1720
extern "C" void d2c_b1720(uint32_t u, const cl::Env& e) {
    if (!u) return;
    uint32_t row = e.UnitRow(u);
    if (row) {
        int32_t s = e.R16s(row + 0x12);
        if (s >= 0 && e.H(0x225d0, 0, 0, 0, 0, 0, (uint32_t)s, u)) e.H(0x88c20, u, 0);
    }
    if (e.H(0xc3d6, u)) e.H(0xa85f0);
    uint32_t type = e.R(u), id = e.R(u + 0xc);
    uint32_t base = ((e.R(u + 0xc4) >> 21) & 1) ? 0xB2000000u + 0x109a08 : 0xB2000000u + 0x10a608;
    e.H(0xa6ef0, id, type, 0, 0, 0, 0, (type << 9) + base, id & 0x7f);
}
