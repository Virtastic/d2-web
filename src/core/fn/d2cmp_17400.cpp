// d2cmp+0x17400 sub_17400: FUNCTION-mode probe.
#include <emscripten.h>
#include <cstdint>
extern "C" void d2cmp_17400(void* cache, void* node, const void* memory);

// r[0] = memory (ECX), r[1] = node or 0 (EAX), r[2] = cache (EDI)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_17400(uint32_t* r) {
    d2cmp_17400((void*)(uintptr_t)r[2], (void*)(uintptr_t)r[1], (const void*)(uintptr_t)r[0]);
    r[4] = 0;
}
