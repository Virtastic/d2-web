// 1.13c D2Game+0x4E500: stack (u, a1, st, -, -), ret 0x14. u: dword +0 must be 1, +4 key; st: +0x4C counter > 0. Takes a slice of the counter off the stat-6 value of
// the unit u maps to. Callees: 0x21260 (ecx), 0xDFA90 (eax), #10973 (thunk 0xA42A), 0x1220 (eax, record with int16 +0x170), 0x1C50 (eax), 0x14D0 (ecx, edx), #10887 (0xA472).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4e500(uint32_t u, uint32_t a1, uint32_t st, const D2Seam* e)
{
    (void)a1;
    if (!u || !st || d2rd32(u) != 1 || (int32_t)d2rd32(st + 0x4C) <= 0) return 0;
    const uint32_t ebp = e->call(0x21260, 0, u);
    if (!ebp) return 0;
    if (e->call(0xDFA90, ebp)) return 0;
    int32_t cur = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, ebp, 6, 0);
    if (cur < 0x100) return 0;
    const uint32_t rec = e->call(0x1220, d2rd32(u + 4));
    if (!rec) return 0;
    const int32_t v = (int16_t)d2rd16(rec + 0x170);
    if (v < 0) return 0;
    e->call(0x1C50, (uint32_t)v);
    const uint32_t d = e->call(0x14D0, 0, 100, d2rd32(st + 0x4C));
    cur = (int32_t)((uint32_t)cur - d);
    if (cur < 0x100) cur = 0x100;
    e->call(0xA472, 0, 0, 0, 0, 0, ebp, 6, (uint32_t)cur, 0);
    const int32_t n = (int32_t)(d2rd32(st + 0x4C) - d);
    d2wr32(st + 0x4C, n > 0 ? (uint32_t)n : 0u);
    return 1;
}
