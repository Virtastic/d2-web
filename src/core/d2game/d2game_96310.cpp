// 1.13c D2Game+0x96310: ecx = c, edx = unit, stack idx, a1 (ret 8). idx picks a skills row (dt+0xB98, stride 0x23C, count dt+0xBA0), else 0; 0x9EE50(eax = 0, ecx = 0, edx = &buf; unit, 0, idx, a1) < 0 returns 0.
// A unit gets bit 0x40 on unit +0xC4; 0x9FA30(eax = unit, ecx = c, ebx = &X; &Y) must succeed; N = 0x95D50(eax = unit; c, 0, 0, row, idx, a1) must be nonzero. Then SetUnitStat(N, 0xC, GetUnitStat(unit, 0xC, 0), 0),
// 0xE19C0(eax = 0x1130(unit), ecx = 0x1100(unit), edx = room(unit); c, N, 0, 0), a seven-dword block {0, 0, 0, 0x1100, 0x1130, X, Y} for 0x21290(eax = c, ecx = N; &block), a walk of N's list (+0xDC, next +0x24) calling 0x8E6F0 on type-2 nodes,
// 0x8EA80(ecx = c, edx = N; 2, c +0xA8 + 1, 0, 0, 0) and N +0xC4 &= ~8; returns 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_96310(uint32_t c, uint32_t unit, uint32_t idx, uint32_t a1, uint32_t dt, const D2Seam* e)
{
    if ((int32_t)idx < 0 || (int32_t)idx >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = idx * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return 0;
    uint32_t buf[8] = {0};
    if ((int32_t)e->call(0x9EE50, 0, 0, (uint32_t)(uintptr_t)buf, 0, 0, unit, 0, idx, a1) < 0) return 0;
    if (unit) d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x40);
    uint32_t X = 0, Y = 0;
    if (!e->call(0x9FA30, unit, c, 0, (uint32_t)(uintptr_t)&X, 0, (uint32_t)(uintptr_t)&Y)) return 0;
    const uint32_t N = e->call(0x95D50, unit, 0, 0, 0, 0, c, 0, 0, row, idx, a1);
    if (!N) return 0;
    const uint32_t v = e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    e->call(0xA472, 0, 0, 0, 0, 0, N, 0xC, v, 0);
    const uint32_t r30 = e->call(0x1130, 0, unit), r00 = e->call(0x1100, 0, unit), room = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    e->call(0xE19C0, r30, r00, room, 0, 0, c, N, 0, 0);
    uint32_t f[7] = {0, 0, 0, 0, 0, X, Y};
    f[3] = e->call(0x1100, 0, unit);
    f[4] = e->call(0x1130, 0, unit);
    e->call(0x21290, c, N, 0, 0, 0, (uint32_t)(uintptr_t)f);
    e->blk(0x21290, f, 7);
    for (uint32_t n = d2rd32(N + 0xDC); n;) {
        const uint32_t next = d2rd32(n + 0x24);
        if (d2rd8(n) == 2) e->call(0x8E6F0, n);
        n = next;
    }
    e->call(0x8EA80, 0, c, N, 0, 0, 2, d2rd32(c + 0xA8) + 1, 0, 0, 0);
    d2wr32(N + 0xC4, d2rd32(N + 0xC4) & 0xFFFFFFF7u);
    return 1;
}
