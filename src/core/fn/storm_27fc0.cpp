// storm+0x27FC0 FN probe: r[0] = seed (EAX); blob tbl (r[14]) is the table the module global points at.
#include <emscripten.h>
#include <cstdint>
uint32_t Storm_Sub27FC0(uint32_t* t, uint32_t seed);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_27fc0(uint32_t* r) {
    r[4] = Storm_Sub27FC0((uint32_t*)(uintptr_t)r[14], r[0]);
}
