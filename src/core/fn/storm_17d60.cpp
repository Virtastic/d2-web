// storm+0x17d60 sgdi_cache_release: FUNCTION-mode probe.
// The port is (src/core/storm/storm_batch_b51.cpp:Storm_Sub17D60); the probe calls it, never reimplements it.
#include <emscripten.h>
#include <cstdint>

uint32_t Storm_Sub17D60();

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_17d60(uint32_t* r) {
    r[4] = Storm_Sub17D60();
}
