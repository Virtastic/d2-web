// 1.13c D2Client+0x2EFD0: skill bonus line. EAX = skill id, stack s0 (EBX for the callee), s1 percent, s2 flat (ret 0xC). Disassembly-derived.
// skills row = *(dt+0xA78) + id * 0x1A8 (id in [0, *(dt+0xA80)), a zero row returns). b = byte [0x11C390]; mode = dword [0x11C394];
// arg = (mode in 2..3) ? dword [0x119858] : 1; lvl = s16 row[0xAA + b*2]. 0xC244(id, arg, b, lvl, 1, &out[14 dwords, zeroed]) (D2Common
// skill value range, stdcall 6) fills out[0] = lo, out[1] = hi. avg = (lo + hi) / 2 (trunc); v = avg + avg * s1 / 100 + s2; v != 0 then 0x2DCA0(0x10C6, v, 0; EBX = s0).
// Globals read: gl = module range from 0x119858 (the dword at +0, the mode and b at 0x11C390/0x11C394).
#include "cl_env.h"
extern "C" void d2cl_2efd0(int32_t skill, uint32_t s0, int32_t pct, int32_t flat, const cl::Env& e) {
    if (skill < 0 || skill >= (int32_t)e.R(e.dt + 0xa80)) return;
    const uint32_t row = (uint32_t)skill * 0x1a8 + e.R(e.dt + 0xa78);
    if (!row) return;
    const uint32_t b = e.R8(e.gl + (0x11c390 - 0x119858));
    uint32_t out[14] = {0};
    const int32_t mode = (int32_t)e.R(e.gl + (0x11c394 - 0x119858));
    const uint32_t arg = (mode >= 2 && mode <= 3) ? e.R(e.gl) : 1u;
    const int32_t lvl = e.R16s(row + 0xaa + b * 2);
    e.Xs((uint32_t)(uintptr_t)out);
    if (!e.H(0xc244, (uint32_t)skill, arg, b, (uint32_t)lvl, 1)) return;
    const int32_t sum = (int32_t)out[0] + (int32_t)out[1];
    int32_t avg = (sum - (sum >> 31)) >> 1;
    const int64_t m = (int64_t)avg * pct;
    int32_t v = avg + (int32_t)(m / 100) + flat;
    if (!v) return;
    e.H(0x2dca0, 0x10c6, (uint32_t)v, 0, 0, 0, 0, 0, 0, s0);
}
