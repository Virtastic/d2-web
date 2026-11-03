// 1.13c D2Client+0x15AB0: fastcall (ECX owner, EDX table index), one stack argument (ret 4), EBX is a callee-save push beside the 0xC2DA stdcall (one stack word), not an argument.
// Row = *(dt+0xB98) + index * 0x23C (index in [0, dt+0xBA0)). Gate: 0x2A0B0(eax owner); window = owner[0x2C] (0: error report, 0x3664(-1));
// target = 0xC2DA(window); first = 0x40550(eax index, owner) in [0, dt+0xB6C). Position (2 * target - owner) from the 0x1630 / 0x1660
// getters, count = 0xC316(owner, row[0x138], index, arg); count times 0xB30F0 on a 23-dword block {0x21, owner, 0, 0, first, tx, ty,
// 2tx - ox, 2ty - oy, ..., [11] index, [12] arg, [21] 0x6FB23B60, [22] iteration}.
#include "cl_env.h"
extern "C" uint32_t d2cl_15ab0(uint32_t owner, int32_t index, uint32_t ebx, uint32_t arg, const cl::Env& e) {
    if (index < 0 || index >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    const uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)index * 0x23c;
    if (!row) return 0;
    e.H(0x2a0b0, 0, 0, 0, 0, 0, owner);
    uint32_t w = e.R(owner + 0x2c);
    if (!w) {
        const uint32_t a = e.H(0xbf92, 0x946);
        e.H(0xbf80, 0x6fb8359f, a);
        w = e.H(0x3664, 0xffffffffu);
    }
    const uint32_t t = e.H(0xc2da, w);   // Ordinal 10392(window), ret 4; EBX is only the saved word beneath it (pushed at 0x15B1B, popped at the exit)
    if (!t) return 0;
    const int32_t first = (int32_t)e.H(0x40550, owner, 0, 0, 0, 0, (uint32_t)index);
    if (first < 0 || first >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    const uint32_t tx = e.H(0x1630, 0, 0, 0, 0, 0, 0, t);
    const uint32_t ty = e.H(0x1660, 0, 0, 0, 0, 0, 0, t);
    const uint32_t ox = e.H(0x1630, 0, 0, 0, 0, 0, 0, owner);
    const uint32_t bx = tx * 2u - ox;
    const uint32_t oy = e.H(0x1660, 0, 0, 0, 0, 0, 0, owner);
    const uint32_t by = ty * 2u - oy;
    const int32_t n = (int32_t)e.H(0xc316, owner, e.R(row + 0x138), (uint32_t)index, arg);
    uint32_t d[23] = {0};
    d[0] = 0x21; d[1] = owner; d[4] = (uint32_t)first; d[5] = tx; d[6] = ty; d[7] = bx; d[8] = by;
    d[11] = (uint32_t)index; d[12] = arg; d[21] = 0x6fb23b60u;
    for (int32_t i = 0; i < n; ++i) {
        d[22] = (uint32_t)i;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
    }
    return 1;
}
