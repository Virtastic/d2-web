// 1.13c D2Client+0xB6D40 (536 B): scatter-spawn over a disc, from the cached decompile.
// Retail registers: EAX = table index, ESI = unit* (seed at +0x20/+0x24); stack: x,y,radius,a,b,thin,step.
// Imports/callees are host seams; ordinal 10331 is unverified (args unknown, called with none).
#include <cstdint>
#include <emscripten.h>

struct D2ClientB6d40Env {
    uint8_t* tbl;                       // *(sgptDataTables)  (+0xb64 base ptr, +0xb6c count; stride 0x1a4)
    uint32_t (*lookupUnit)(void);       // D2Client+0x560D0, returns 0 for none
    void (*ord10331)(void);             // unverified ordinal
    int32_t (*sub1400)(void);           // D2Client+0x1400
    void (*sub_b30f0)(uint32_t* rec);   // D2Client+0xB30F0, rec = 0x17 dwords
};

static uint32_t b6d40_roll(uint32_t* seed, uint32_t n) {   // seed[0]=lo, seed[1]=hi
    uint64_t v = (uint64_t)seed[0] * 0x6ac690c5u + seed[1];
    seed[0] = (uint32_t)v; seed[1] = (uint32_t)(v >> 32);
    return (n & (n - 1)) == 0 ? (n - 1) & (uint32_t)v : (uint32_t)v % n;
}

// 1.13c D2Client+0xB6D40
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b6d40(
    int32_t idx, uint32_t* unit, int32_t x, int32_t y, int32_t r,
    uint32_t a, uint32_t b, uint32_t thin, int32_t step, D2ClientB6d40Env* e) {
    uint32_t cnt = *(uint32_t*)(e->tbl + 0xb6c);
    uint32_t tblBase = *(uint32_t*)(e->tbl + 0xb64);
    uint32_t extra = 0;
    if (idx >= 0 && (uint32_t)idx < cnt) extra = *(uint32_t*)((uintptr_t)tblBase + idx * 0x1a4 + 0x17c);
    if (!(unit && (unit[0] == 0 || unit[0] == 1)) && !e->lookupUnit()) return;
    uint32_t rec[0x17] = {};
    rec[0] = 0x201; rec[0xb] = a; rec[0xc] = b;
    for (int32_t dy = -r; dy <= r; dy += step) {
        for (int32_t dx = -r; dx <= r; dx += step) {
            if (dx * dx + dy * dy > r * r) continue;
            if ((int32_t)thin > 0 && b6d40_roll(unit + 8, thin) != 0) continue;
            rec[5] = x + dx; rec[6] = y + dy;
            if (extra) rec[0x10] = (int32_t)extra > 0 ? b6d40_roll(unit + 8, extra) : 0;
            e->ord10331();
            if (e->sub1400()) e->sub_b30f0(rec);
        }
    }
}
