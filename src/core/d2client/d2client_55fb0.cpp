// 1.13c D2Client+0x55FB0 -- per-slot summon dispatch for a type-4 unit (EDI = unit, EAX = ctx; stack p1, p2; ret 8; void).
// Needs a unit of type 4 with 0xC19C (unit) == 5 and q = 0xCCB8 (unit); mode = byte q+0x87.
// Mode 1: mask = 0xCCAC (ctx, unit, 0), me = 0xCC9A (unit); each slot i < 6 other than me whose mask bit is set gets
//   0x55B20 (ECX = 0; unit, p1, p2, 0, T[i - (i > me)], 0, 1). T is .rdata 0xD2E50 {0xA5..0xAA}; T[-1] is the 0xFFFFFFFF before it.
// Mode 2: v = 0xCCAC (ctx, unit, 1), n = popcount(v) when v < 0x40 (the .rdata 0xD5D20 table), else 0; slots 0..n-2 get the call.
#include "cl_env.h"
namespace {
const uint32_t kSlotM1[7] = { 0xffffffff, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa };   // [0] is .rdata 0xD2E4C
void call55b20(const cl::Env& e, uint32_t unit, uint32_t p1, uint32_t p2, uint32_t t) {
    const uint32_t w[7] = { unit, p1, p2, 0, t, 0, 1 };
    e.Xs(0, 1);
    e.H(0x55b20, w[0], w[1], w[2], w[3], w[4], 0, 0);
    if (e.blk) e.blk(e.ctx, 0x55b20, (uint32_t)(uintptr_t)w, 7);
}
}
extern "C" void d2c_55fb0(uint32_t unit, uint32_t ctx, uint32_t p1, uint32_t p2, const cl::Env& e) {
    if (!unit || e.R(unit) != 4) return;
    if (e.H(0xc19c, unit) != 5) return;
    uint32_t q = e.H(0xccb8, unit);
    if (!q) return;
    uint32_t m = e.R8(q + 0x87);
    if (m == 1) {
        uint32_t mask = e.H(0xccac, ctx, unit, 0);
        int32_t me = (int32_t)e.H(0xcc9a, unit);
        for (int32_t i = 0; i < 6; i++) {
            if (i == me) continue;
            int32_t off = i > me ? i - 1 : i;
            if (mask & (1u << i)) call55b20(e, unit, p1, p2, kSlotM1[off + 1]);
        }
    } else if (m == 2) {
        uint32_t v = e.H(0xccac, ctx, unit, 1);
        int32_t n = v < 0x40 ? __builtin_popcount(v) : 0;
        for (int32_t i = 0; i < n - 1; i++) call55b20(e, unit, p1, p2, kSlotM1[i + 1]);
    }
}
