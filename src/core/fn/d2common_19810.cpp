// 1.13c D2Common+0x19810; r[0] = key, r[1] = owner blob; void
// 1.13c D2Common+0x19860: the mirror, same probe shape
#include <emscripten.h>
#include <cstdint>
static uint32_t* P(const uint8_t* b, uint32_t o) { return (uint32_t*)(uintptr_t)*(const uint32_t*)(b + o); }
static void run(uint32_t key, const uint8_t* a, uint32_t clr, uint32_t set) {
    for (const uint8_t* n = (const uint8_t*)P(a, 0x4C); n; n = (const uint8_t*)P(n, 4)) {
        if (*P(n, 0x10) != key) continue;
        const uint8_t* c = (const uint8_t*)P(n, clr);
        if (!c) return;
        for (; c; c = (const uint8_t*)P(c, 0x20)) *(uint32_t*)(c + 0x14) &= ~8u;
        for (c = (const uint8_t*)P(n, set); c; c = (const uint8_t*)P(c, 0x20)) *(uint32_t*)(c + 0x14) |= 8u;
        return;
    }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_19810(uint32_t* r) { run(r[0], (const uint8_t*)(uintptr_t)r[1], 0xC, 0x14); r[4] = 0; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_19860(uint32_t* r) { run(r[0], (const uint8_t*)(uintptr_t)r[1], 0x14, 0xC); r[4] = 0; }
