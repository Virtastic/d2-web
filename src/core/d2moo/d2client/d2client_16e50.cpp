// 1.13c D2Client+0x16E50 -- nearest-unit visitor: a candidate that is not the reference unit, is a player or a monster
// other than classes 0x95 and 0x22C, and that the reference filter does not reject, replaces the best candidate when
// its distance from the reference is below the best so far. Always returns 0.
#include "cl_tbl.h"
extern "C" uint32_t d2c_16e50(uint32_t ref, uint32_t u, uint32_t pBest, uint32_t pDist, const cl::Env& e) {
    if (ref == u || !u) return 0;
    const uint32_t ty = e.R(u);
    if (ty != 0) {
        if (ty != 1) return 0;
        const uint32_t id = e.R(u + 4);
        if (id == 0x95 || id == 0x22c) return 0;
    }
    // 1.13c D2Client+0xA5440 inline (EAX = ref): flag 0x10000 in [ref+0xC4] or a mode of [ref+0x10] that is 0 or the
    // type's idle mode (0x11 player, 0xC monster) rejects; the distance call logs stack words only.
    const uint32_t rmode = e.R(ref + 0x10), rty = e.R(ref);
    if (e.R(ref + 0xc4) & 0x10000u) return 0;
    if (rty == 0 ? (rmode == 0 || rmode == 0x11) : (rty == 1 && (rmode == 0 || rmode == 0xc))) return 0;
    const uint32_t d = e.H(0xc190, ref, u);
    if ((int32_t)d < (int32_t)e.R(pDist)) {
        e.W(pBest, u);
        e.W(pDist, d);
    }
    return 0;
}
