// 1.13c D2Client+0xA09F0 -- visitor over units with a state block at +8 of the record: the unit's id (dword at +0xC,
// -1 without a unit; compared unsigned) above the pivot at +0x10 and below the upper best at +4 becomes the upper
// best (unit at +0); otherwise an id not above the lower best at +0xC becomes the lower best (unit at +8). The
// counter at +0x14 goes up either way. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_a09f0(uint32_t rec, uint32_t u, const cl::Env& e) {
    const uint32_t s = e.R(rec + 8);
    const uint32_t id = u ? e.R(u + 0xc) : 0xFFFFFFFFu;
    if (id > e.R(s + 0x10) && id < e.R(s + 4)) {
        e.W(s + 4, id);
        const uint32_t n = e.R(s + 0x14) + 1;
        e.W(s, u);
        e.W(s + 0x14, n);
        return 1;
    }
    if (id <= e.R(s + 0xc)) {
        e.W(s + 0xc, id);
        e.W(s + 8, u);
    }
    e.W(s + 0x14, e.R(s + 0x14) + 1);
    return 1;
}
