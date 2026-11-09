// 1.13c D2Client+0xC6060, written from the disassembly. ESI = unit, EBX = a source, stack s0 (ret 4). path = [unit+0x2C], row = 10331(unit); both
// must be nonzero. A player (type 1) uses 0x100 / 0x3C01 / mode 2, anything else 0x80 / 0x1C09 / mode 7: when EBX and s0 are nonzero
// 11073(row, EBX, s0, 10123(unit), size) runs, then 11056(path, size), 11164(path, tag), 10647(path, mode). The unit is then returned to its
// rest state through 10084(unit, 0) when [unit+0xC4] has bit 0x10000, or type 0 with mode 0 / 0x11, or type 1 with mode 0 / 0xC. Returns nothing.
#include "cl_mb2.h"
// 1.13c D2Client+0xC6060
extern "C" uint32_t d2c_c6060(uint32_t u, uint32_t ebx, uint32_t s0, const clm::Env& e) {
    const uint32_t type = u ? e.R(u) : 6u;
    const uint32_t path = e.R(u + 0x2c);
    const uint32_t row = e.H(0xc1f0, 0, 0, 0, 0, 0, u);
    if (!path || !row) return 0;
    const bool pl = type == 1;
    const uint32_t size = pl ? 0x100u : 0x80u;
    if (ebx && s0) e.H(0xcb98, 0, 0, 0, 0, 0, row, ebx, s0, e.H(0xcd30, 0, 0, 0, 0, 0, u), size);
    e.H(0xc400, 0, 0, 0, 0, 0, path, size);
    e.H(0xc3ac, 0, 0, 0, 0, 0, path, pl ? 0x3c01u : 0x1c09u);
    e.H(0xc38e, 0, 0, 0, 0, 0, path, pl ? 2u : 7u);
    if (!(e.R(u + 0xc4) & 0x10000)) {
        const uint32_t mode = e.R(u + 0x10);
        if (e.R(u) == 0) { if (mode && mode != 0x11) return 0; }
        else if (e.R(u) == 1) { if (mode && mode != 0xc) return 0; }
        else return 0;
    }
    e.H(0xcd12, 0, 0, 0, 0, 0, u, 0);
    return 0;
}
