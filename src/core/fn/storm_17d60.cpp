// storm+0x17d60 sgdi_cache_release: FUNCTION-mode probe.
// The port is (src/core/storm/storm_batch_b51.cpp:Storm_Sub17D60); the probe calls it, never reimplements it.
#include <emscripten.h>
#include <cstdint>

uint32_t Storm_Sub17D60();
extern "C" uint32_t* Storm_RetailGlobalAddr(uint32_t rva);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_17d60(uint32_t* r) {
    r[4] = Storm_Sub17D60();
}

// State-init writes: a retail global maps to its backing dword in storm_retail_globals.cpp.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fnw_storm_17d60(uint32_t globalRva) {
    return (uint32_t)(uintptr_t)Storm_RetailGlobalAddr(globalRva);
}
