// Shared pieces of the D2Client 0xA2xxx skill-effect ports: the 0xB9A80 request call and the effects tail of 0xA2950 / 0xA2C90.
#pragma once
#include "cl_env.h"
namespace cl {
// call 0xB9A80 with EAX = unit and the 8 request words on the stack (the log keeps the first four plus the 8-dword block)
inline uint32_t b9a80(const Env& e, uint32_t unit, const uint32_t* w) {
    const uint32_t r = e.H(0xb9a80, w[0], w[1], w[2], w[3], 0, unit);
    if (e.blk) e.blk(e.ctx, 0xb9a80, (uint32_t)(uintptr_t)w, 8);
    return r;
}
// skill-row effects shared by 0xA2950 and 0xA2C90 (rec = skills row)
inline void a2Effects(const Env& e, uint32_t unit, uint32_t rec) {
    if (e.R16s(rec + 0x100) > 0) e.H(0x88a70, unit, 0, 0, 0, 0, 0, 0, 0, (uint32_t)e.R16s(rec + 0x100));
    if (e.R16s(rec + 0x10a) > 0) {
        const uint32_t hld = e.H(0xa5f70);
        if (hld) e.H(0x88a70, hld, 0, 0, 0, 0, 0, 0, 0, (uint32_t)e.R16s(rec + 0x10a));
    }
    if (e.R16s(rec + 0x108) > 0) {
        const uint32_t hld = e.H(0xa5f70);
        if (hld) { e.Xs(0, 0); e.H(0x6b300, hld, 2, 0, 0, 0, (uint32_t)e.R16s(rec + 0x108)); }
    }
}
}  // namespace cl
