// 1.13c D2Client+0x16E50 -- nearest-unit visitor: a candidate that is not the reference unit, is a player or a monster
// other than classes 0x95 and 0x22C, and that the reference filter (0xA5440) does not reject, replaces the best candidate
// when its distance from the reference is below the best so far. Always returns 0.
// The filter is a leaf that only reads the unit (EAX = unit; flags +0xC4, type +0, mode +0x10) and leaves ECX / EDX alone,
// so it is read inline: the reference then stays in ECX / EDX for the distance call's first stack word.
#include "cl_tbl.h"
static uint32_t filter_a5440(const cl::Env& e, uint32_t u) {   // 1.13c D2Client+0xA5440
    if (e.R(u + 0xc4) & 0x10000u) return 1;
    const uint32_t ty = e.R(u), mode = e.R(u + 0x10);
    if (ty == 0) return (mode == 0 || mode == 0x11) ? 1 : 0;
    return (ty == 1 && (mode == 0 || mode == 0xc)) ? 1 : 0;
}
extern "C" uint32_t d2c_16e50(uint32_t ref, uint32_t u, uint32_t pBest, uint32_t pDist, const cl::Env& e) {
    if (ref == u || !u) return 0;
    const uint32_t ty = e.R(u);
    if (ty != 0) {
        if (ty != 1) return 0;
        const uint32_t id = e.R(u + 4);
        if (id == 0x95 || id == 0x22c) return 0;
    }
    if (filter_a5440(e, ref)) return 0;
    const uint32_t d = e.H(0xc190, ref, u);
    if ((int32_t)d < (int32_t)e.R(pDist)) {
        e.W(pBest, u);
        e.W(pDist, d);
    }
    return 0;
}
