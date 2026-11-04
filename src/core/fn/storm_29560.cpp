// storm+0x29560 FN probe: no args; blob G (r[14]) = module words 0x53110..0x5312F (thread at +0, event at +0x1C).
#include <emscripten.h>
#include <cstdint>
extern "C" void storm_29560_fnrun(uint32_t* g);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_29560(uint32_t* r) {
    storm_29560_fnrun((uint32_t*)(uintptr_t)r[14]);
    r[4] = 0;
}
