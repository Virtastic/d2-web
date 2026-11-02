// storm+0x1d4d0 bitmap_rec_shutdown: FUNCTION-mode probe.
// The port is (src/core/storm/storm_batch_b51.cpp:Storm_Sub1D4D0); the probe calls it, never reimplements it.
#include <emscripten.h>
#include <cstdint>

uint32_t Storm_Sub1D4D0();

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1d4d0(uint32_t* r) {
    r[4] = Storm_Sub1D4D0();
}
