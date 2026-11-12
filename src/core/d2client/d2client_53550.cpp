// 1.13c D2Client+0x53550 -- scatter up to three placement requests inside a unit's area. stdcall(unit). Needs the seed object at global 0x6FBCBBFC (null: nothing happens).
// Request block (23 dwords): [0] 0x2003, [1] seed object, [4] 0x11F, [5] x, [6] y, [11] 0, [12] 1. Area = Ordinal_10561(unit) giving {x0, y0, w, h}. At most 25 tries; each draws
// x = x0 + rnd(w), y = y0 + rnd(h) from the object's 64-bit LCG at +0x20 (lo) / +0x24 (hi): p = lo*0x6AC690C5 + hi, lo' = low32(p), hi' = high32(p);
// rnd(n) = 0 for n <= 0, (n-1) & lo' for a power of two, lo' % n otherwise. Ordinal_10482(unit, x, y, 5) == 0 submits the block (0xB30F0, ecx = block) and counts toward 3.
#include <cstdint>
struct D2c53550Env {
    void (*area)(uint32_t unit, uint32_t* out4);
    int32_t (*test)(uint32_t unit, uint32_t x, uint32_t y, uint32_t five);
    void (*submit)(uint32_t* req);
};
static uint32_t rnd(uint8_t* seed, int32_t n) {
    if (n < 1) return 0;
    uint32_t& lo = *(uint32_t*)(seed + 0x20); uint32_t& hi = *(uint32_t*)(seed + 0x24);
    const uint64_t p = (uint64_t)lo * 0x6ac690c5u + hi;
    lo = (uint32_t)p; hi = (uint32_t)(p >> 32);
    return ((uint32_t)n & ((uint32_t)n - 1)) == 0 ? ((uint32_t)n - 1) & lo : lo % (uint32_t)n;
}
// The block's x and y are words 5 and 6 (word 4 keeps 0x11F). EAX on return: the submit count (the loop exits with it); a null seed object
// leaves the caller's EAX, returned here as 0.
extern "C" uint32_t d2c_53550r(uint32_t unit, uint8_t* seed, const D2c53550Env& e) {
    if (!seed) return 0;
    uint32_t req[23] = {};
    req[0] = 0x2003; req[1] = (uint32_t)(uintptr_t)seed; req[11] = 0; req[12] = 1; req[4] = 0x11f;
    uint32_t a[4]; e.area(unit, a);
    int32_t succ = 0;
    for (int32_t tries = 0; tries < 0x19; ++tries) {
        req[5] = a[0] + rnd(seed, (int32_t)a[2]);
        req[6] = a[1] + rnd(seed, (int32_t)a[3]);
        if ((int16_t)e.test(unit, req[5], req[6], 5) == 0) { e.submit(req); ++succ; }
        if (succ >= 3) break;
    }
    return (uint32_t)succ;
}
