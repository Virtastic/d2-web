// storm+0x2ae30 bitset compare: FUNCTION-mode probe. The port is storm_batch_2ad10.cpp:Storm_2AE30; the probe calls it.
#include <emscripten.h>
#include <cstdint>
#include "../storm/storm_big_retail.h"

int32_t Storm_2AE30(const Storm2fVector* left, const Storm2fVector* right);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_2ae30(uint32_t* r) {
    const Storm2fVector left = Storm2fVectorFromRetail(r[0]);
    const Storm2fVector right = Storm2fVectorFromRetail(r[1]);
    r[4] = (uint32_t)Storm_2AE30(&left, &right);
}
