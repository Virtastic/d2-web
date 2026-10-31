// 1.13c D2CMP+0x3B03 _realloc: replays the content-hash protocol of the retail capture.
#include <emscripten.h>
#include <cstdint>
#include <cstdlib>
extern "C" void* d2cmp_malloc(size_t n);
extern "C" void* d2cmp_realloc(void* p, size_t n);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_3b03(uint32_t* r) {   // r[0] = old size (0 = NULL), r[1] = n; r[4] = FNV of the result block
    const uint32_t old = r[0], n = r[1];
    uint8_t* p = nullptr;
    if (old) {
        p = (uint8_t*)d2cmp_malloc(old);
        if (!p) { r[4] = 0xFFFFFFFFu; return; }
        for (uint32_t i = 0; i < old; ++i) p[i] = (uint8_t)(i * 31u + old);
    }
    uint8_t* q = (uint8_t*)d2cmp_realloc(p, n);
    if (!q) { r[4] = 0; return; }
    const uint32_t lim = old < n ? old : n;
    uint32_t h = 0x811C9DC5u;
    for (uint32_t i = 0; i < lim; ++i) h = (h ^ q[i]) * 0x01000193u;
    free(q);
    r[4] = h ^ 0x80000000u;
}
