// d2cmp+0x171e0 sub_171E0: FUNCTION-mode probe.
#include <emscripten.h>
#include <cstdint>
extern "C" void* d2cmp_171e0(void* cache, uint32_t size);

// r[0] = cache (ECX), r[1] = size delta (EDI); r[4] = node or 0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_171e0(uint32_t* r) {
    r[4] = (uint32_t)(uintptr_t)d2cmp_171e0((void*)(uintptr_t)r[0], r[1]);
}
