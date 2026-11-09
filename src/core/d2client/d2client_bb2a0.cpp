// 1.13c D2Client+0xbb2a0 -- missile frame-tick spawn (the sound/effect child every d frames). ECX = unit, no return.
// u == 0, Missiles row invalid or s16 row+0x1E < 0: 0xB1720(ESI = u), return. c = 10240(u) (0xC388) must index the 0x23C-byte
// skill table at [dt+0xB98] (count [dt+0xBA0]), else 0xB1720(ESI = u), return. m = 11029(u) (0xC3FA), p = 0xA60D0(EAX = u).
// n = row+0x5C, when <= 0: 10786(p, sk+0x64, c, m) (0xC316) clamped to >= 1; d = row+0x58, when <= 0: 10786(p, sk+0x144, c, m) clamped.
// f = 10985(u) (0xC4D2); row+0x60 < f < row+0x64 and f % d == 0 (signed): seed = u+0x20, 0xC3B2(ECX = seed, EDX = 11018(u) (0xC322));
// dx = 0x1330(ECX = seed, EAX = 2n+1) - n + 0x1630(ECX = u); dy = 0x1330(..) - n + 0x1660(ECX = u); 10254(u, 0xC4AE(ECX = seed)) (0xC448);
// 0xB9A80(EAX = u; s16 row+0x1E, 0, 0, dx, dy, 10240(u), m, row+0x68) (logged by `hookblk 0xb9a80 k 8`). Tail 0xB17C0(ECX = u).
#include <cstdint>
#include "cl_env.h"
extern "C" void d2c_bb2a0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t c = e.H(0xc388, u);
    uint32_t sk = 0;
    if ((int32_t)c >= 0 && (int32_t)c < (int32_t)e.R(e.dt + 0xba0)) sk = e.R(e.dt + 0xb98) + c * 0x23c;
    if (!sk) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t m = e.H(0xc3fa, u);
    uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    int32_t n = (int32_t)e.R(row + 0x5c);
    if (n <= 0) { n = (int32_t)e.H(0xc316, p, e.R(sk + 0x64), c, m); if (n < 1) n = 1; }
    int32_t d = (int32_t)e.R(row + 0x58);
    if (d <= 0) { d = (int32_t)e.H(0xc316, p, e.R(sk + 0x144), c, m); if (d < 1) d = 1; }
    int32_t f = (int32_t)e.H(0xc4d2, u);
    if (f > (int32_t)e.R(row + 0x60) && f < (int32_t)e.R(row + 0x64) && f % d == 0) {
        const uint32_t seed = u + 0x20;
        uint32_t v = e.H(0xc322, u);
        e.H(0xc3b2, 0, 0, 0, 0, 0, 0, seed, v);
        uint32_t k = 2 * (uint32_t)n + 1;
        uint32_t dx = e.H(0x1330, 0, 0, 0, 0, 0, k, seed) - (uint32_t)n;
        dx += e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        uint32_t dy = e.H(0x1330, 0, 0, 0, 0, 0, k, seed) - (uint32_t)n;
        dy += e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        e.H(0xc448, u, e.H(0xc4ae, 0, 0, 0, 0, 0, 0, seed));
        uint32_t r68 = e.R(row + 0x68);
        uint32_t c2 = e.H(0xc388, u);
        uint32_t w[8] = { (uint32_t)e.R16s(row + 0x1e), 0, 0, dx, dy, c2, m, r68 };
        e.H(0xb9a80, w[0], w[1], w[2], w[3], w[4], u);
        if (e.blk) e.blk(e.ctx, 0xb9a80, (uint32_t)(uintptr_t)w, 8);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
