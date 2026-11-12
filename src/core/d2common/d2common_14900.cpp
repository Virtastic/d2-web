// 1.13c D2Common+0x14900 -- overlap test (ecx = the work record, edx = n) of the preset table at image offset 0x8CCE0: answers 0 when rectangle n overlaps an earlier rectangle other than its anchor; otherwise, for n != 0, answers whether rectangle 0 grown 200 units upward stays clear of rectangle n (the record is restored).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
static int32_t gap(int32_t a, int32_t al, int32_t b, int32_t bl) { return a < b ? (int32_t)((uint32_t)b - (uint32_t)al - (uint32_t)a) : (int32_t)((uint32_t)a - (uint32_t)bl - (uint32_t)b); }
static bool hit(GE& e, uint32_t r, uint32_t q) { return gap((int32_t)e.rd32(r), (int32_t)e.rd32(r + 8), (int32_t)e.rd32(q), (int32_t)e.rd32(q + 8)) < 0 && gap((int32_t)e.rd32(r + 4), (int32_t)e.rd32(r + 12), (int32_t)e.rd32(q + 4), (int32_t)e.rd32(q + 12)) < 0; }
uint32_t f14900(GE& e, uint32_t t, uint32_t n)
{
    static const int32_t skip[15] = { -1, -1, 1, 2, 3, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };   // the anchor of each rectangle of this preset
    int32_t i = 0;
    for (; i < (int32_t)n; i++) {
        if (n < 15 && i == skip[n]) continue;
        if (hit(e, t + 8 + n * 16, t + 8 + (uint32_t)i * 16)) return 0;
    }
    if (!n) return 1;
    const uint32_t h0 = e.rd32(t + 0x14);
    e.wr32(t + 0xc, e.rd32(t + 0xc) - 200);
    e.wr32(t + 0x14, h0 + 200);
    const uint32_t r = hit(e, t + 8, t + 8 + (uint32_t)i * 16) ? 0 : 1;
    e.wr32(t + 0x14, h0);
    e.wr32(t + 0xc, e.rd32(t + 0xc) + 200);
    return r;
}
}
