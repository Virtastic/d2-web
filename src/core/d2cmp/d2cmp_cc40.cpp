// D2CMP SpriteCache node acquire (evicting). EAX = cache in retail, stack (size, payload, alt), ret 0xC. The cache and its nodes are
// 32-bit retail addresses (flat guest image); the in-module callees, the Fog pool calls, the lock and the destroy callback are seams.
// Cache: +0 pool, +4 limit, +8 used bytes, +0xc evictable count, +0x14/+0x18 queue A head/tail, +0x1c/+0x20 queue B head/tail,
// +0x24/+0x28 free list head/tail, +0x2c destroy callback, +0x30 critical section, +0x34 total, +0x38 live.
// Node: +0 data, +4 size, +8 payload, +0xc flags (bit 0 alt, bit 1 pinned), +0x10 tick, +0x14 next, +0x18 prev.
#include <cstdint>
#include <cstring>

struct D2CmpCc40Env {
    uint32_t (*getFree)(uint32_t size, uint32_t cache, uint32_t alt);   // 0xC030: EBX = size, EDI = cache, stack alt (ret 4)
    uint32_t (*popQueue)(uint32_t older, uint32_t cache);               // 0xBF40: EAX = B head older than A head, EDX = cache
    uint32_t (*alloc)(uint32_t pool, uint32_t size);                    // Fog 10045 (thunk 0x8D84): SpriteCache.cpp line 0x99d
    void (*release)(uint32_t pool, uint32_t data);                      // Fog 10046 (thunk 0x8D96): SpriteCache.cpp line 0xa22
    void (*destroy)(uint32_t fn, uint32_t payload);                     // call [cache+0x2c], ECX = payload
    uint32_t (*tick)();                                                 // GetTickCount
    void (*enter)(uint32_t cs);
    void (*leave)(uint32_t cs);
    void (*fatal)(uint32_t code);                                       // Fog 10265 / 10024 / _exit(-1)
};

static inline uint32_t rdk(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wrk(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }

// Pop the head of the queue at cache+ho (tail at ho+4); 0 when it is empty or its head is pinned.
static uint32_t takeHead(uint32_t c, uint32_t ho)
{
    const uint32_t v = rdk(c + ho);
    if (!v || (rdk(v + 0xc) & 2)) return 0;
    const uint32_t nx = rdk(v + 0x14);
    wrk(c + ho, nx);
    if (nx) wrk(nx + 0x18, 0); else wrk(c + ho + 4, 0);
    wrk(v + 0x14, 0); wrk(v + 0x18, 0);
    return v;
}

// 1.13c D2CMP+0x0CC40
uint32_t d2cmp_cc40(uint32_t c, uint32_t size, uint32_t payload, uint32_t alt, const D2CmpCc40Env* e)
{
    if (size >= rdk(c + 4)) { e->fatal(0x971); return 0; }
    int32_t stamp = 0;
    if (alt) {
        if (rdk(c + 0x14)) stamp = (int32_t)rdk(rdk(c + 0x14) + 0x10);
        if (rdk(c + 0x1c)) { const int32_t b = (int32_t)rdk(rdk(c + 0x1c) + 0x10); if (!(stamp < b)) stamp = b; }
    }
    for (;;) {
        const uint32_t n = e->getFree(size, c, alt);
        if (n) {
            const uint32_t f = rdk(n + 0xc);
            wrk(n + 0xc, alt ? (f | 1) : (f & ~1u));
            wrk(n, e->alloc(rdk(c), size));
            wrk(n + 4, size);
            wrk(n + 8, payload);
            wrk(c + 0x38, rdk(c + 0x38) + 1);
            wrk(c + 0x34, rdk(c + 0x34) + 1);
            return n;
        }
        if (alt && e->tick() - (uint32_t)stamp < 15000u) return 0;
        if (!rdk(c + 0xc)) return 0;
        uint32_t v;
        if (!rdk(c + 0x1c)) v = takeHead(c, 0x14);
        else if (!rdk(c + 0x14)) v = takeHead(c, 0x1c);
        else v = e->popQueue(rdk(rdk(c + 0x1c) + 0x10) < rdk(rdk(c + 0x14) + 0x10) ? 1u : 0u, c);
        if (!v || (rdk(v + 0xc) & 2) || !rdk(c + 0x30)) return 0;
        e->enter(rdk(c + 0x30));
        wrk(c + 8, rdk(c + 8) - rdk(v + 4));
        e->leave(rdk(c + 0x30));
        if (rdk(v + 8)) { e->destroy(rdk(c + 0x2c), rdk(v + 8)); wrk(v + 8, 0); }
        if (rdk(v)) { e->release(rdk(c), rdk(v)); wrk(v, 0); }
        const uint32_t ft = rdk(c + 0x28);
        if (!ft) wrk(c + 0x24, v);
        else { wrk(ft + 0x14, v); wrk(v + 0x18, rdk(c + 0x28)); }
        wrk(c + 0x28, v);
        wrk(c + 0xc, rdk(c + 0xc) - 1);
    }
}
