// 1.13c Storm+0x9198 (CRT calloc, cdecl count size): the port's block is recorded by content (resb 16): r[4] is its address,
// the replay compares its 16 bytes (zero) against retail's; the previous tuple's block is released first.
#include <emscripten.h>
#include <cstdint>
#include <cstdlib>
extern "C" void* Storm_Sub9198(uint32_t n, uint32_t sz);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_9198(uint32_t* r) {
    static void* prev = nullptr;
    free(prev);
    prev = Storm_Sub9198(r[0], r[1]);
    r[4] = (uint32_t)(uintptr_t)prev;
}
