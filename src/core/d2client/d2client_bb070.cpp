// 1.13c D2Client+0xbb070 -- missile multi-sound set: up to three Missiles row words drive 0xB9740. ECX = unit, always returns 1.
// u == 0 or Missiles row invalid: 1. c = 10240(u) (0xC388), m = 11029(u) (0xC3FA); c must index the 0x23C-byte skill table at
// [dt+0xB98] (count [dt+0xBA0]), else 1. arr = the s16 words at row+0x2C, +0x2E, +0x30 up to the first negative one (cnt of them);
// cnt == 0: 1. n = row+0x6C, when <= 0: 10786(0xA60D0(EAX = u), sk+0x138, c, m) (0xC316) clamped to >= 1.
// r = 10231(u, 0xA60D0(EAX = u), row+0x88, u+4 class, m) (0xC2C2); r <= 0: r = s16 row+0x98 * (m - 1) + s16 row+0x96 (32-bit wrap).
// 0xB9740(u, &arr, cnt, n, r, row+0x70) (ret 0x18). The capture logs it by `hookblk 0xb9740 k 15 MASK`: the 6 stack words,
// the caller's 4 saved registers zeroed, m, c, then arr with the unfilled words zeroed (MASK per tuple).
#include <cstdint>
#include "cl_env.h"
extern "C" uint32_t d2c_bb070(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    uint32_t row = e.UnitRow(u);
    if (!row) return 1;
    uint32_t c = e.H(0xc388, u), m = e.H(0xc3fa, u);
    uint32_t sk = 0;
    if ((int32_t)c >= 0 && (int32_t)c < (int32_t)e.R(e.dt + 0xba0)) sk = e.R(e.dt + 0xb98) + c * 0x23c;
    if (!sk) return 1;
    uint32_t w[15] = {};
    uint32_t* arr = w + 12;
    uint32_t cnt = 0;
    while (cnt < 3) {
        int32_t v = e.R16s(row + 0x2c + 2 * cnt);
        if (v < 0) break;
        arr[cnt++] = (uint32_t)v;
    }
    if (!cnt) return 1;
    int32_t n = (int32_t)e.R(row + 0x6c);
    if (n <= 0) {
        uint32_t s138 = e.R(sk + 0x138);
        uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        n = (int32_t)e.H(0xc316, p, s138, c, m);
        if (n < 1) n = 1;
    }
    uint32_t r88 = e.R(row + 0x88), cls = e.R(u + 4);
    uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    int32_t r = (int32_t)e.H(0xc2c2, u, p, r88, cls, m);
    if (r <= 0) r = (int32_t)((uint32_t)e.R16s(row + 0x98) * (m - 1) + (uint32_t)e.R16s(row + 0x96));
    w[0] = u; w[1] = (uint32_t)(uintptr_t)arr; w[2] = cnt; w[3] = (uint32_t)n; w[4] = (uint32_t)r; w[5] = e.R(row + 0x70);
    w[10] = m; w[11] = c;
    e.H(0xb9740, w[0], w[1], w[2], w[3], w[4]);
    if (e.blk) e.blk(e.ctx, 0xb9740, (uint32_t)(uintptr_t)w, 15);
    return 1;
}
