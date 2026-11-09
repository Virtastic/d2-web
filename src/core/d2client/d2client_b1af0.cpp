// 1.13c D2Client+0xb1af0 -- client missile per-tick dispatch. EAX = unit, no return.
// No unit / bad class: 0xB1720(ESI = unit). Byte row +0x131: 0xB0DF0(ECX = unit). room = 0xC1F0(unit); none: y / x = the path
// coordinate (static-path types read path +0x10 / +0xC, others 0xC2EC / 0xC3BE(path), 0 without a path), room =
// 0x1400(EAX = 0, EBX = x, EDI = y). When (byte gdwBitMasks[2] & byte row +5) == 0 and 0xC208(room) != 0: 0xB1720(ESI = unit).
// Otherwise idx = row +8 (signed word); 0 < idx < [0xDDA0C] (91): tail-jump through the table at 0xDD8A0 with ECX = unit.
#include "cl_env.h"
namespace {
const uint32_t kTbl[91] = {   // 0xDD8A0 (rva; 0 = empty slot, retail jumps to a null entry)
    0,       0xb17c0, 0xb3060, 0xbbe60, 0xbbe00, 0xb2f90, 0xb92b0, 0xb2eb0, 0xb91c0, 0xb8f00,
    0xbbd40, 0xb2dc0, 0xb2d00, 0xbbc10, 0xb2a80, 0xb8bc0, 0xb8aa0, 0xbbb40, 0xb8960, 0xb8840,
    0xb29b0, 0xb86f0, 0xb8550, 0xb2980, 0xbb8f0, 0xb83d0, 0xbb780, 0xb8130, 0xb8010, 0xbc210,
    0xb79f0, 0xb4b80, 0xb2340, 0xb4760, 0xb1fc0, 0xb44a0, 0xb41f0, 0xb2500, 0xba7e0, 0xb2480,
    0xba880, 0xb24a0, 0xba680, 0xb2890, 0xb2790, 0xbb500, 0xb7880, 0xbb470, 0xb75c0, 0xb2720,
    0xbb2a0, 0xb73e0, 0xb72d0, 0xb7220, 0xb9c10, 0xb3860, 0xb70a0, 0xb6f60, 0xb25d0, 0xb1cc0,
    0xb1c00, 0xba600, 0xba4a0, 0xb1f20, 0xb9ff0, 0xb1d10, 0xb9ea0, 0xb2bf0, 0xb77a0,
};
}
extern "C" void d2c_b1af0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    if (e.R8(row + 0x131)) e.H(0xb0df0, 0, 0, 0, 0, 0, 0, u);
    uint32_t room = e.H(0xc1f0, u);
    if (!room) {
        uint32_t y = e.PD(u, 0xc2ec, 0x10);
        uint32_t x = e.PD(u, 0xc3be, 0xc);
        room = e.H(0x1400, 0, 0, 0, 0, 0, 0, 0, 0, x);
        e.Edj(0x1400, y);
    }
    if ((e.R8(e.bm + 8) & e.R8(row + 5)) == 0 && e.H(0xc208, room)) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t idx = e.R16s(row + 8);
    if (idx <= 0 || idx >= 91 || !kTbl[idx]) return;
    e.H(kTbl[idx], 0, 0, 0, 0, 0, 0, u);
}
