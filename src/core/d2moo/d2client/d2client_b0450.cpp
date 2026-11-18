// 1.13c D2Client+0xB0450 (esi = unit; five stack args, ret 0x14): build the 0x5C-byte request block and hand it to 0xB30F0 (ecx)
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0450
extern "C" uint32_t d2c_b0450(uint32_t esi, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, const cl::Env& e) {
    if (!esi) return 0;
    uint32_t t = e.R(esi), unit = esi;
    if (t != 0 && t != 1) {
        unit = e.h(e.ctx, 0xa60d0, esi, 0, 0, 0, 0, 0, 0, 0, 0, 0);          // lookup, eax = unit, plain ret
        if (!unit) return 0;
    }
    uint32_t blk[23] = {};
    blk[0] = 0x20; blk[1] = unit; blk[2] = esi;
    blk[4] = a0; blk[7] = a1; blk[8] = a2; blk[11] = a3; blk[12] = a4;
    return e.h(e.ctx, 0xb30f0, 0, (uint32_t)(uintptr_t)blk, 0, 0, 0, 0, 0, 0, 0, 0);   // ecx = block, plain ret
}
