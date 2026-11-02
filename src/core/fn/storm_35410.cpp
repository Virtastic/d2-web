// storm+0x35410 UTF-8 decode: FUNCTION-mode probe. The port is storm_batch_z11.cpp:Storm_35410; the probe calls it.
#include <emscripten.h>
#include <cstdint>

uint32_t Storm_35410(const uint8_t* p, int* used);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_35410(uint32_t* r) {
    r[4] = Storm_35410((const uint8_t*)(uintptr_t)r[0], (int*)(uintptr_t)r[1]);
}
