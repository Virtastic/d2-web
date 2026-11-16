// 1.13c D2Client+0x8DAD0 (482 bytes): an item's damage range for the tooltip.
// EAX = item; stdcall unit, statMin, statMax, outMin, outMax, outFlag; `ret 0x18`. Retail 32-bit layout; the D2Common
// imports go through the D2cHost seam in retail's call order, stack words as retail pushes them:
//   0xC51A #11160(gBC38, &slot)  0xC4FC #10164(owner, item, n)  0xC544 #11127(owner, item)  0xC1C6 #10973(item, stat, 0)
//   0xC250 #10587(item, stat, 0)  0xC5FE #11044(owner+0x1C, &k)  0xC8C2 #10449(stat, t, k, 0, 0, 0)
// 0x14A0 (a*b/c with the overflow shortcuts) runs as our port. Globals: 0x11BC38 (gBC38); *sgptDataTables +0xBD4
// (the stat-table row count) and +0xBCC (the stat-table base, tested as base+0x15840 / base+0x15984 != 0).
#include <cstdint>
#include <cstring>
#include "client_host.h"

extern "C" int32_t d2_client_14a0(int32_t a, int32_t b, int32_t d);

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
uint32_t call(D2cHost h, uint32_t rva, uint32_t s0, uint32_t s1, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0) {
    D2cCall c{ rva, 0, 0, 0, 0, 0, { s0, s1, s2, s3 }, { 0, 0, 0, 0 }, s4, 0 };
    return h(&c);
}
}  // namespace

struct D2c8dad0Globals {
    uint32_t gBC38;      // DAT 0x11BC38
    int32_t  statRows;   // *sgptDataTables +0xBD4
    uint32_t statBase;   // *sgptDataTables +0xBCC
};

// 1.13c D2Client+0x8DAD0
extern "C" void d2_client_8dad0(uint32_t item, uint32_t unit, uint32_t sMin, uint32_t sMax, uint32_t outMin,
                                uint32_t outMax, uint32_t outFlag, const D2c8dad0Globals* g, D2cHost h) {
    wr32(outMin, 1);
    wr32(outMax, 2);
    if (!unit || !item) return;
    uint32_t slot = 0;
    const uint32_t r = call(h, 0xC51A, g->gBC38, (uint32_t)(uintptr_t)&slot);
    const uint32_t owner = (r && r != unit) ? r : unit;
    call(h, 0xC4FC, owner, item, 1);
    const int32_t lo = (int32_t)call(h, 0xC1C6, item, sMin, 0);
    int32_t hi = (int32_t)call(h, 0xC1C6, item, sMax, 0);
    if (!r) call(h, 0xC544, owner, item);
    else call(h, 0xC4FC, owner, item, slot);
    const int32_t bLo = (int32_t)call(h, 0xC250, item, sMin, 0);
    const int32_t bHi = (int32_t)call(h, 0xC250, item, sMax, 0);
    if (bLo < lo || bHi < hi) wr32(outFlag, 1);
    if (hi <= lo) hi = lo;
    uint32_t st = call(h, 0xC1C6, item, 0x110, 0);
    if (st && g->statRows > 0x110 && g->statBase + 0x15840u && rd32(owner + 0x1C)) {
        uint32_t k = 0;
        const uint32_t t = call(h, 0xC5FE, rd32(owner + 0x1C), (uint32_t)(uintptr_t)&k);
        const int32_t v = (int32_t)call(h, 0xC8C2, st, t, k, 0, 0);
        hi += v;
        if (v) wr32(outFlag, 1);
    }
    st = call(h, 0xC1C6, item, 0x111, 0);
    if (st && g->statRows > 0x111 && g->statBase + 0x15984u && rd32(owner + 0x1C)) {
        uint32_t k = 0;
        const uint32_t t = call(h, 0xC5FE, rd32(owner + 0x1C), (uint32_t)(uintptr_t)&k);
        const int32_t v = (int32_t)call(h, 0xC8C2, st, t, k, 0, 0);
        hi += d2_client_14a0(v, hi, 100);
        if (v) wr32(outFlag, 1);
    }
    wr32(outMin, (uint32_t)lo);
    wr32(outMax, (uint32_t)hi);
}
