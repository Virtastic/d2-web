// storm+0x28DF0 FN probe: arg0 = file struct (blob f, r[0]), arg1 = offset (r[1]); the port's cache fill.
#include <emscripten.h>
#include <cstdint>
extern "C" uint32_t storm_28df0_fnrun(void* f, uint32_t off);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_28df0(uint32_t* r) {
    r[4] = storm_28df0_fnrun((void*)(uintptr_t)r[0], r[1]);
}
