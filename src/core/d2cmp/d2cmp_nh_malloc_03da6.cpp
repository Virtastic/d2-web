// 1.13c D2CMP+0x03da6 (44 B, caller 0x3DD2): VC7.1 CRT __nh_malloc(size, nhflag). Seams: 0x6315
// __heap_alloc, 0x3D2B __callnewh.
#include <cstdint>
#include <emscripten.h>

struct D2cmpNhMallocEnv {
    void*   (*heap_alloc)(uint32_t size);
    int32_t (*callnewh)(uint32_t size);
};

extern "C"
EMSCRIPTEN_KEEPALIVE void* d2cmp_nh_malloc_03da6(uint32_t size, int32_t nhflag, const D2cmpNhMallocEnv* env) {
    if (size >= 0xffffffe1u) return nullptr;
    for (;;) {
        void* p = env->heap_alloc(size);
        if (p) return p;
        if (nhflag == 0 || env->callnewh(size) == 0) return nullptr;
    }
}
