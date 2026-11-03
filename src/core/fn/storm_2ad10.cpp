// storm+0x2ad10 bitset find: FUNCTION-mode probe. The port is storm_batch_2ad10.cpp:Storm_2AD10 (the Storm2fVector layout of storm_big_types.h); the probe calls it.
#include <emscripten.h>
#include <cstdint>
#include "../storm/storm_big_retail.h"

uint32_t Storm_2AD10(const Storm2fVector* value);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_2ad10(uint32_t* r) {
    const Storm2fVector v = Storm2fVectorFromRetail(r[0]);
    r[4] = Storm_2AD10(&v);
}
