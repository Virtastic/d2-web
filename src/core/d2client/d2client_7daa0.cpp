// 1.13c D2Client+0x7DAA0 (344 B, fastcall ECX = unit, void): unit name label, the quest/mission variant of 0x7DC00 without the
// 0xFFFF-prefixed name branch. id = [unit+0xA4]; position via 0xC21A with y shifted by -10 - [0x113AD8]; on screen: the id's
// text (0xCAF0 into a 0x110-byte buffer, atol 0x39C4) must be 1..0xFFFE, D2Lang string 0xD1EC(atol) non-null, then label draw.
#include "cl_namelabel.h"
// 1.13c D2Client+0x7DAA0
extern "C" void d2c_7daa0(uint32_t unit, uint32_t ga, uint32_t gm, uint32_t gs, const cl::Env& e) {
    int32_t pos[2] = {0, 0};
    uint32_t id;
    if (!cl::labelPos(e, unit, ga, gm, gs, -10, pos, id)) return;
    uint8_t nb[0x110] = {};
    e.H(0xcaf0, id, (uint32_t)(uintptr_t)nb);
    const int32_t l = (int32_t)e.H(0x39c4, (uint32_t)(uintptr_t)nb);
    if (l <= 0 || l >= 0xffff) return;
    const uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, (uint32_t)l);
    if (!s) return;
    uint16_t buf[200];
    cl::labelDraw(e, unit, id, pos, s, 0, buf, 200);
}
