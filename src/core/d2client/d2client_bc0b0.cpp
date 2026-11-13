// 1.13c D2Client+0xBC0B0 (ecx = unit, edx = target): missile row 0xB0450 (+0x2C word, when non-negative) and 0xBB210 (+0x2E word, row +0x6C/+0x70, 1) with the unit's 0xC388/0xC3FA values; a nonzero target takes 0x6B300(target, 2) with eax 0x97; with a room, a nonzero target and a positive row +0x36 word, 0x6B300(room, 2) with eax = that word. Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_bc0b0(uint32_t u, uint32_t tgt, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row) return 1;
    uint32_t x = e.H(0xc388, u);
    uint32_t y = e.H(0xc3fa, u);
    if (e.R16s(row + 0x2c) >= 0) e.H(0xb0450, (uint32_t)e.R16s(row + 0x2c), 0, 0, x, y);
    if (e.R16s(row + 0x2e) >= 0) {
        uint32_t w[2] = { e.R(row + 0x70), 1 };
        e.H(0xbb210, e.R(row + 0x6c), (uint32_t)e.R16s(row + 0x2e), x, y, w[0], u);
        e.Xw(0xbb210, w, 2);
    }
    if (tgt) e.H(0x6b300, tgt, 2, 0, 0, 0, 0x97);
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (room && tgt && e.R16s(row + 0x36) > 0) e.H(0x6b300, room, 2, 0, 0, 0, (uint32_t)e.R16s(row + 0x36));
    return 1;
}
