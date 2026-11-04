// d2cmp+0x12e60 sub_12E60: FUNCTION-mode probe.
#include <emscripten.h>
#include <cstdint>
extern "C" uint32_t d2cmp_12e60(const void* index, uint32_t key1c, const void** out,
                                uint32_t cap, uint32_t key14, uint32_t key18);

// r[0] = index (EAX), r[1] = key1c (EBX), r[2] = out (stack 1), r[3] = cap (stack 2),
// r[5] = key14 (stack 3), r[6] = key18 (stack 4); r[4] = result count
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_12e60(uint32_t* r) {
    r[4] = d2cmp_12e60((const void*)(uintptr_t)r[0], r[1], (const void**)(uintptr_t)r[2],
                       r[3], r[5], r[6]);
}
